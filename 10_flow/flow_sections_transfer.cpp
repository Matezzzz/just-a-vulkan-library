#include "flow_sections_transfer.h"





FlowStagingBuffer::FlowStagingBuffer(FlowContext& ctx, VkDeviceSize buf_size) : StagingBuffer{buf_size}, m_context{ctx}, m_pool{CommandPoolInfo{0}.create()}
{
    m_submit_sync.addEndFence();
}
void FlowStagingBuffer::transfer(Queue& transfer_q, FlowSection* section, uint64_t timeout){
    //allocate a new command buffer from the pool, record the given FlowSection on it
    CommandBuffer cmd_buf{m_pool.allocateBuffer()};
    cmd_buf.startRecordPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    section->run(cmd_buf);
    cmd_buf.endRecord();
    //submit the recorded command buffer, then wait for timeout until the operation finishes
    transfer_q.submit(cmd_buf, m_submit_sync);
    m_submit_sync.waitFor(timeout);
    //reset the command buffer, do not release resources
    m_pool.reset(false);
}