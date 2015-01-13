#include "steg.h"

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Read filename returning its content in dynamically allocated memory,
 *  with its total data length in *lenP.
 */
static unsigned char *
read_file(const char *filename, int *lenP)
{
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "cannot open file %s: %s\n", filename, strerror(errno));
    exit(1);
  }
  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "cannot seek on file %s: %s\n", filename, strerror(errno));
    exit(1);
  }
  int len = ftell(f);
  if (len < 0) {
    fprintf(stderr, "cannot tell file position on file %s: %s\n",
            filename, strerror(errno));
    exit(1);
  }
  rewind(f);
  unsigned char *data = malloc(len);
  if (!data) {
    fprintf(stderr, "cannot get memory for file data: %s\n", strerror(errno));
    exit(1);
  }
  if (fread(data, 1, len, f) != len) {
    fprintf(stderr, "cannot read file %s: %s\n", filename, strerror(errno));
    exit(1);
  }
  if (fclose(f) != 0) {
    fprintf(stderr, "cannot close file %s: %s\n", filename, strerror(errno));
    exit(1);
  }
  *lenP = len;
  return data;
}


static int
get_data_offset(unsigned char data[], int dataLen)
{
  enum { N_WHITESPACE_SEGMENTS = 4 }; //quit after seeing this many ws segments
  int nWhitespaceSegmentsSeen = 0;
  int i;
  for (i = 0; i < dataLen; i++) {
    if (isspace(data[i])) {
      nWhitespaceSegmentsSeen++;
      if (nWhitespaceSegmentsSeen == N_WHITESPACE_SEGMENTS) break;
      while (isspace(data[i])) {
        i++;
      }
    }
  }
  if (nWhitespaceSegmentsSeen < N_WHITESPACE_SEGMENTS) {
    fprintf(stderr, "image data file format error\n");
    exit(1);
  }
  return i + 1; //last whitespace segment contains exactly 1 ws char
}

/** Encode message msg into contents of filename and output it encoded
 *  file content on standard output.  Return 0 iff everything ok.
 */
static int
encode(const char *filename, const char *msg)
{
  int dataLen;
  unsigned char *data = read_file(filename, &dataLen);
  int msgLen = strlen(msg);
  int dataOffset = get_data_offset(data, dataLen);
  int ret = steg_encode((unsigned char *)msg, msgLen,
                        &data[dataOffset], dataLen - dataOffset);
  if (ret != msgLen) {
    fprintf(stderr, "steganography encode error: message too large\n");
    return 1;
  }
  if (fwrite(data, 1, dataLen, stdout) != dataLen) {
    fprintf(stderr, "cannot write encoded data to stdout\n");
    return 1;
  }
  free(data);
  return 0;
}

/** Decode steganograph'd message from filename and output it on
 *  standard output.  Return 0 iff everything ok.
 */
static int
decode(const char *filename)
{
  int dataLen;
  unsigned char *data = read_file(filename, &dataLen);
  int maxMsgLen = (1 << MSG_LEN_BITS);
  unsigned char msg[maxMsgLen];
  int dataOffset = get_data_offset(data, dataLen);
  int msgLen = steg_decode(&data[dataOffset], dataLen - dataOffset,
                           msg, maxMsgLen);
  free(data);
  if (msgLen < 0) {
    fprintf(stderr, "cannot decode steganography message\n");
    return 1;
  }
  printf("%.*s\n", msgLen, msg);
  return 0;
}

/** main program: check command-line args and call appropriate
 *  decode or encode routine.
 */
int
main(int argc, const char *argv[])
{
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "usage: %s IMG_FILE_NAME [MESSAGE]\n", argv[0]);
    exit(1);
  }
  return (argc == 2) ? decode(argv[1]) : encode(argv[1], argv[2]);
}