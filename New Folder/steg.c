#include "steg.h"
#include <stdio.h>

#include <limits.h> //defines CHAR_BIT: # of bits per char

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
int
steg_encode(const unsigned char msg[], int msgLen,
            unsigned char data[], int dataLen)
{ 
  int result = 1;
  int c,i,k;
  int msgLenBinary[MSG_LEN_BITS];
  int counter = 0;
  //return -1 if the message length is too big
  for (i = 0; i < MSG_LEN_BITS; i++)
  {
    result *= 2;
  }
  if (msgLen <= result)
    {
      //convert message length into binary
     for (c = MSG_LEN_BITS-1; c >= 0; c--)
    {
      k = msgLen >> c;
   
      if (k & 1)
      {
        msgLenBinary[c] = 1;
      }
      else
      {
        msgLenBinary[c] = 0;
      }
    }
   //encode message length in data[]
   //use counter to not overlap in data
    for (i = 0; i < MSG_LEN_BITS; i++)
    {
      counter++;
      if(msgLenBinary[i] == 0)
      {
        data[i] &= ~0x1;
      }
      else
      {
        data[i] |= 0x1;
      }
    }
    //encode msg in data
    //change LSB
    for(i = 0; i < msgLen; i++)
    {
    	for(int binIndex = 0; binIndex < (CHAR_BIT); binIndex++)
    	{
    		int changeBit = ((msg[i] >> binIndex) & 1);
    		if (changeBit == 1)
    		{
    			data[counter] |= changeBit << 0;
    		}
    		else{
    			data[counter] &= ~0x1;
    		}
    		counter++;
    	}
    }
  	  return msgLen;
    }
    //return < 0 if msgLen can't be encoded
    return -1;
}


/** Decode previously encoded message from data[] (of total length
 *  dataLen chars) into msg[] array of length msgLen chars.  Return
 *  length of decoded message if everything ok, otherwise return < 0
 *  (msgLen not large enough to hold decoded message).
 */
int
steg_decode(const unsigned char data[], int dataLen,
            unsigned char msg[], int msgLen)
{
  int i;
  int dataMsgLen = 0;
  //loop to find least significant bit of the first bits of data with MSG_LEN_BITS
  //bit shift to find msgLen
  for (i = 0; i < MSG_LEN_BITS; i++)
  {
    int LSB = ((data[i] >>0)&1);
    dataMsgLen |= LSB << i;
  }
  //return -1 if msgLen not large enough
  if (dataMsgLen > msgLen)
  {
    return -1;
  }
  //decode actual message
  int msgDec = 0;
	int msgBinDec = 0;
	unsigned char ch = 0;
	int totalMsgLen = ((CHAR_BIT * dataMsgLen) + CHAR_BIT);
	//loop to read LSB
	for(int i = MSG_LEN_BITS; i < totalMsgLen; i++)
	{
		int LSB = ((data[i] >> 0) & 1);
		ch |= LSB << msgBinDec;
		//letter has to be CHAR_BIT length
		if(msgBinDec < (CHAR_BIT-1))
		{
			msgBinDec++;
		}
		//clear the variables for loop and add the letter to msg
		else
		{
			msgBinDec = 0;
			msg[msgDec] = ch;
			msgDec++;
			ch = 0;
		}
	}
	//if msgLen not large enough return -1
	if((msgDec+1) > msgLen)
	{
		return -1;
	}
	return dataMsgLen;
}

