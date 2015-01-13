#ifndef _STEG_H_
#define _STEG_H_

/** # of bits used for encoding message length.  It follows that max msg
 *  length cannot exceed 2**MSG_LEN_BITS - 1 (** denotes exponentiation).
 */
enum { MSG_LEN_BITS = 4 }; //low-value permits easier testing error conditions

/** Encode msg[] of length msgLen in data[] array of length dataLen.  Use least-
 *  significant bits of data to encode msg bits as follows:
 *
 *   First encode length of message using MSG_LEN_BITS data[] chars,
 *   LSB first.
 *
 *   Then encode msg[] chars using CHAR_BIT * msgLen data[] chars with
 *   the bits in each msg char being encoded LSB first.
 *
 *   Return # of msg chars encoded if everything ok (should be
 *   msgLen); on error (dataLen is not large enough to encode msgLen
 *   chars) return < 0.
 */
int steg_encode(const unsigned char msg[], int msgLen,
                unsigned char data[], int dataLen);

/** Decode previously encoded message from data[] (of total length
 *  dataLen chars) into msg[] array of length msgLen chars.  Return
 *  length of decoded message if everything ok, otherwise return < 0
 *  (msgLen not large enough to hold decoded message).
 */
int steg_decode(const unsigned char data[], int dataLen,
                unsigned char msg[], int msgLen);

#endif