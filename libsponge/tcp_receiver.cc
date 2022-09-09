#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    auto header = seg.header();
    auto payload = seg.payload();


    // 当前不是建立状态，则丢弃所有非建立连接的segment
    if (this->state != SYN) {
        if(!header.syn)
            return;
        this->state = SYN;
        this->_isn = header.seqno;
    }

    // 当前绝对ack序号
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    // 当前数据段的wrap序号转为绝对序号
    // checkpoint 为当前unassembled的最后一个
    uint64_t cur_seqno = unwrap(header.seqno, _isn, abs_ackno);

    // payload写入，注意index需要根据SYN和FIN多加1
    // 需要-1
    uint64_t stream_idx = header.syn ? 0 : cur_seqno - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_idx, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(this->state != SYN)
        return nullopt;

    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    if(_reassembler.stream_out().input_ended())
        abs_ackno++;
    return WrappingInt32(_isn) + abs_ackno;
}

size_t TCPReceiver::window_size() const { return _capacity-_reassembler.stream_out().buffer_size(); }
