#ifndef FLOW_SECTIONS_TRANSFER
#define FLOW_SECTIONS_TRANSFER

#include "../10_flow/flow_sections.h"





/**
 * @brief Allow calling the overloaded versions of transfer method based on the amount of parameters in variadic template AdditionalArgs
 * 
 */
namespace FlowTransferSectionInternal{

    //has only one method, transfer, given an object, and template argument for the amount of parameters, calls a target transfer method
    template<int argn, typename TransferT>
    class TransferWrapped;

    template<typename TransferT>
    class TransferWrapped<0, TransferT>{
    public:
        static void transfer(CommandBuffer& buf, const TransferT& t){
            t.transfer(buf, t.m_source, t.m_target);  
        }
    };
    template<typename TransferT>
    class TransferWrapped<1, TransferT>{
    public:
        static void transfer(CommandBuffer& buf, const TransferT& t){
            t.transfer(buf, t.m_source, t.m_target, std::get<0>(t.m_args));
        }
    };
}



 
/**
 * @brief Parent class for all transfer flow sections. Automatically creates barriers, then transfers during execute
 * 
 * @tparam FromTClass must define required methods and static variables - see TransferImageClass, TransferBufferClass for examples
 * @tparam ToTClass must define required methods and static variables - see TransferImageClass, TransferBufferClass for examples
 * @tparam AdditionalArgs will be saved, then passed to the transfer method
 */
template<typename FromTClass, typename ToTClass, typename... AdditionalArgs>
class FlowTransferSection : public FlowTransitionSection{
public:
    //where to copy from
    typename FromTClass::type_saved m_source;

    //where to copy to
    typename ToTClass::type_saved m_target;

    //saved arguments to pass to transfer
    const std::tuple<AdditionalArgs...> m_args;
    constexpr static int additional_arg_count = sizeof...(AdditionalArgs);
public:
    FlowTransferSection(FlowContext& ctx, typename FromTClass::type used_src, typename ToTClass::type used_dst, AdditionalArgs... args) :
        FlowTransitionSection(ctx, 
            //create an empty vector, then add source flow transition if it needs one, then add destination flow transition if it needs one
            vector_ext<FlowSectionDescriptorUsage>()
                .addIf(FromTClass::transitions, FlowSectionDescriptorUsage{FromTClass::getIndex(used_src), VK_PIPELINE_STAGE_TRANSFER_BIT, FromTClass::transferSrc})
                .addIf(  ToTClass::transitions, FlowSectionDescriptorUsage{  ToTClass::getIndex(used_dst), VK_PIPELINE_STAGE_TRANSFER_BIT,   ToTClass::transferDst})
        ), m_source{FromTClass::get(ctx, used_src)}, m_target{ToTClass::get(ctx, used_dst)}, m_args{args...}
    {}

    //call the correct transfer method with given args, source and destination
    virtual void execute(CommandBuffer& buffer){
        FlowTransferSectionInternal::TransferWrapped<additional_arg_count, FlowTransferSection<FromTClass, ToTClass, AdditionalArgs...>>::transfer(buffer, *this);
    }

    //transfer method image->image
    static void transfer(CommandBuffer& cmd_buffer,  const Image& from,  const Image& to);

    //transfer method image->buffer
    static void transfer(CommandBuffer& cmd_buffer,  const Image& from, const Buffer& to);

    //transfer method for buffer->buffer. size in bytes.
    static void transfer(CommandBuffer& cmd_buffer, const Buffer& from, const Buffer& to, VkDeviceSize size){
        cmd_buffer.cmdCopyBuffer(from, to, size);
    }
    //transfer method buffer->image. size is set to match the whole image
    static void transfer(CommandBuffer& cmd_buffer, const Buffer& from, const Image& to){
        cmd_buffer.cmdCopyBufferToImage(from, to);
    }
};


//Following four structs are used to specialize FlowTransferSection for common transfers

struct FlowDescriptorTransferClass{
    using type = int;
    static constexpr bool transitions = true;
    static int getIndex(type value) {return value;}
};
struct TransferImageClass : public FlowDescriptorTransferClass{
    using type_saved = const ExtImage&;
    static constexpr ImageState transferSrc {ImgState::TransferSrc};
    static constexpr ImageState transferDst {ImgState::TransferDst};
    static type_saved get(FlowContext& ctx, type index) {return ctx.getImage(index);}
};
struct TransferBufferClass : public FlowDescriptorTransferClass{
    using type_saved = const Buffer&;
    static constexpr BufferState transferSrc {BufState::TransferSrc};
    static constexpr BufferState transferDst {BufState::TransferDst};
    static type_saved get(FlowContext& ctx, type index) {return ctx.getBuffer(index);}
};
struct StagingBufferClass{
    using type = const StagingBuffer&;
    using type_saved = const Buffer&;
    static constexpr BufferState transferSrc {BufState::Invalid};
    static constexpr BufferState transferDst {BufState::Invalid};
    static constexpr bool transitions = false;
    static constexpr type_saved get(FlowContext&, type buf) {return buf;}
    static int getIndex(type) {return -1;}
};

//defines a transfer from a staging buffer to an image
using FlowImageTransferSection = FlowTransferSection<StagingBufferClass, TransferImageClass>;

//defines a transfer from a staging buffer to another buffer
using FlowBufferTransferSection = FlowTransferSection<StagingBufferClass, TransferBufferClass, VkDeviceSize>;








/**
 * @brief Can transfer images and buffers to the GPU, updating their state in FlowContext
 * 
 */
class FlowStagingBuffer : protected StagingBuffer{
    FlowContext m_context;
    CommandPool m_pool;
    SubmitSynchronization m_submit_sync;
public:
    /**
     * @brief Create a flow staging buffer with given context
     * 
     * @param ctx flow context to take descriptors from
     * @param buf_size staging buffer size in bytes
     */
    FlowStagingBuffer(FlowContext& ctx, VkDeviceSize buf_size);

    /**
     * @brief Transfer data from a vector to an image on the GPU
     * 
     * @tparam T type of data in the vector
     * @param transfer_q queue to transfer on
     * @param image destination image
     * @param data source data
     * @param timeout how long to wait for the transfer to finish
     */
    template<typename T>
    void transferImage(Queue& transfer_q, int image, const vector<T>& data, uint64_t timeout = A_SHORT_WHILE){
        copyTo(data);
        FlowImageTransferSection s{m_context, *this, image};
        transfer(transfer_q, &s, timeout);
    }

    /**
     * @brief Transfer data from a vector to a buffer on the GPU
     * 
     * @tparam T type of data in the vector
     * @param transfer_q queue to transfer on
     * @param buffer destination buffer
     * @param data source data
     * @param timeout how long to wait for the transfer to finish
     */
    template<typename T>
    void transferBuffer(Queue& transfer_q, int buffer, const vector<T>& data, uint64_t timeout = A_SHORT_WHILE){
        copyTo(data);
        FlowBufferTransferSection s{m_context, *this, buffer, data.size() * sizeof(T)};
        transfer(transfer_q, &s, timeout);
    }
private:
    /**
     * @brief use given section to transfer data to the GPU
     * 
     * @param transfer_q 
     * @param section 
     * @param timeout 
     */
    void transfer(Queue& transfer_q, FlowSection* section, uint64_t timeout = A_SHORT_WHILE);
};


#endif