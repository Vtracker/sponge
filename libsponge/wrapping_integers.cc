#include "wrapping_integers.hh"
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return WrappingInt32{(isn+static_cast<uint32_t>(n))};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const constexpr uint64_t MOD = 1l << 32;
    uint32_t offset = n - isn;
    if(checkpoint > offset){
        // >= 1次回环
        // ret = offset + n * MOD,n 取最近的
        uint64_t wrap_cnt = (checkpoint - offset + (MOD >> 1)) / MOD;
        uint64_t wrap_cnt2 = wrap_cnt != 0 ? wrap_cnt - 1 : wrap_cnt;

        uint64_t offset_1 = offset + wrap_cnt * MOD;
        uint64_t offset_2 = offset + wrap_cnt2 * MOD;


        uint64_t abs_1 = offset_1 > checkpoint ? offset_1 - checkpoint : checkpoint - offset_1;
        uint64_t abs_2 = offset_2 > checkpoint ? offset_2 - checkpoint : checkpoint - offset_2;
        
        if(abs_1 < abs_2)
            return offset_1;
        return offset_2;
    } else
        return offset;
}
