//
//  TMidasFile.cxx.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include "TMidasFile.h"
#include "TMidasEvent.h"

TMidasFile::TMidasFile()
{
  uint32_t endian = 0x12345678;

  fFile = -1;
  fGzFile = NULL;
  fPoFile = NULL;
  fLastErrno = 0;

  fDoByteSwap = *(char*)(&endian) != 0x78;
}

TMidasFile::~TMidasFile()
{
  Close();
}

static int hasSuffix(const char*name,const char*suffix)
{
  const char* s = strstr(name,suffix);
  if (s == NULL)
    return 0;

  return (s-name)+strlen(suffix) == strlen(name);
}

bool TMidasFile::Open(const char *filename)
{
  /// Open a midas .mid file with given file name.
  ///
  /// Remote files can be accessed using these special file names:
  /// - pipein://command - read data produced by given command, see examples below
  /// - ssh://username\@hostname/path/file.mid - read remote file through an ssh pipe
  /// - ssh://username\@hostname/path/file.mid.gz and file.mid.bz2 - same for compressed files
  /// - dccp://path/file.mid (also file.mid.gz and file.mid.bz2) - read data from dcache, requires dccp in the PATH
  ///
  /// Examples:
  /// - ./event_dump.exe /ladd/data9/t2km11/data/run02696.mid.gz - read normal compressed file
  /// - ./event_dump.exe ssh://ladd09//ladd/data9/t2km11/data/run02696.mid.gz - read compressed file through ssh to ladd09 (note double "/")
  /// - ./event_dump.exe pipein://"cat /ladd/data9/t2km11/data/run02696.mid.gz | gzip -dc" - read data piped from a command or script (note quotes)
  /// - ./event_dump.exe pipein://"gzip -dc /ladd/data9/t2km11/data/run02696.mid.gz" - another way to read compressed files
  /// - ./event_dump.exe dccp:///pnfs/triumf.ca/data/t2km11/aug2008/run02837.mid.gz - read file directly from a dcache pool (note triple "/")
  ///
  /// \param [in] filename The file to open.
  /// \returns "true" for succes, "false" for error, use GetLastError() to see why

  if (fFile > 0)
    Close();

  fFilename = filename;

  std::string pipe;

  // Do we need these?
  //signal(SIGPIPE,SIG_IGN); // crash if reading from closed pipe
  //signal(SIGXFSZ,SIG_IGN); // crash if reading from file >2GB without O_LARGEFILE

  if (strncmp(filename, "ssh://", 6) == 0)
    {
      const char* name = filename + 6;
      const char* s = strstr(name, "/");

      if (s == NULL)
        {
          fLastErrno = -1;
          fLastError = "TMidasFile::Open: Invalid ssh:// URI. Should be: ssh://user@host/file/path/...";
          return false;
        }

      const char* remoteFile = s + 1;

      std::string remoteHost;
      for (s=name; *s != '/'; s++)
        remoteHost += *s;

      pipe = "ssh -e none -T -x -n ";
      pipe += remoteHost;
      pipe += " dd if=";
      pipe += remoteFile;
      pipe += " bs=1024k";

      if (hasSuffix(remoteFile,".gz"))
        pipe += " | gzip -dc";
      else if (hasSuffix(remoteFile,".bz2"))
        pipe += " | bzip2 -dc";
    }
  else if (strncmp(filename, "dccp://", 7) == 0)
    {
      const char* name = filename + 7;

      pipe = "dccp ";
      pipe += name;
      pipe += " /dev/fd/1";

      if (hasSuffix(filename,".gz"))
        pipe += " | gzip -dc";
      else if (hasSuffix(filename,".bz2"))
        pipe += " | bzip2 -dc";
    }
  else if (strncmp(filename, "pipein://", 9) == 0)
    {
      pipe = filename + 9;
    }
#if 0 // read compressed files using the zlib library
  else if (hasSuffix(filename, ".gz"))
    {
      pipe = "gzip -dc ";
      pipe += filename;
    }
#endif
  else if (hasSuffix(filename, ".bz2"))
    {
      pipe = "bzip2 -dc ";
      pipe += filename;
    }

  if (pipe.length() > 0)
    {
      fprintf(stderr,"TMidasFile::Open: Reading from pipe: %s\n", pipe.c_str());

      fPoFile = popen(pipe.c_str(), "r");

      if (fPoFile == NULL)
        {
          fLastErrno = errno;
          fLastError = strerror(errno);
          return false;
        }

      fFile = fileno((FILE*)fPoFile);
    }
  else
    {
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

      fFile = open(filename, O_RDONLY | O_LARGEFILE);

      if (fFile <= 0)
        {
          fLastErrno = errno;
          fLastError = strerror(errno);
          return false;
        }

      if (hasSuffix(filename, ".gz"))
        {
          // this is a compressed file
#ifdef HAVE_ZLIB
          fGzFile = new gzFile;
          (*(gzFile*)fGzFile) = gzdopen(fFile,"rb");
          if ((*(gzFile*)fGzFile) == NULL)
            {
              fLastErrno = -1;
              fLastError = "zlib gzdopen() error";
              return false;
            }
#else
          fLastErrno = -1;
          fLastError = "Do not know how to read compressed MIDAS files";
          return false;
#endif
        }
    }

  return true;
}

static int readpipe(int fd, char* buf, int length)
{
  int count = 0;
  while (length > 0)
    {
      int rd = read(fd, buf, length);
      if (rd > 0)
        {
          buf += rd;
          length -= rd;
          count += rd;
        }
      else if (rd == 0)
        {
          return count;
        }
      else
        {
          return -1;
        }
    }
  return count;
}

bool TMidasFile::Read(TMidasEvent *midasEvent)
{
  /// \param [in] midasEvent Pointer to an empty TMidasEvent 
  /// \returns "true" for success, "false" for failure, see GetLastError() to see why

  midasEvent->Clear();

  int rd = 0;

  if (fGzFile)
#ifdef HAVE_ZLIB
    rd = gzread(*(gzFile*)fGzFile, (char*)midasEvent->GetEventHeader(), sizeof(EventHeader_t));
#else
    assert(!"Cannot get here");
#endif
  else
    rd = readpipe(fFile, (char*)midasEvent->GetEventHeader(), sizeof(EventHeader_t));

  if (rd == 0)
    {
      fLastErrno = 0;
      fLastError = "EOF";
      return false;
    }
  else if (rd != sizeof(EventHeader_t))
    {
      fLastErrno = errno;
      fLastError = strerror(errno);
      return false;
    }

  if (fDoByteSwap)
    midasEvent->SwapBytesEventHeader();

  if (!midasEvent->IsGoodSize())
    {
      fLastErrno = -1;
      fLastError = "Invalid event size";
      return false;
    }

  if (fGzFile)
#ifdef HAVE_ZLIB
    rd = gzread(*(gzFile*)fGzFile, midasEvent->GetData(), midasEvent->GetDataSize());
#else
    assert(!"Cannot get here");
#endif
  else
    rd = readpipe(fFile, midasEvent->GetData(), midasEvent->GetDataSize());

  if (rd != (int)midasEvent->GetDataSize())
    {
      fLastErrno = errno;
      fLastError = strerror(errno);
      return false;
    }

  midasEvent->SwapBytes(false);

  return true;
}

void TMidasFile::Close()
{
  if (fPoFile)
    pclose((FILE*)fPoFile);
  fPoFile = NULL;
#ifdef HAVE_ZLIB
  if (fGzFile)
    gzclose(*(gzFile*)fGzFile);
  fGzFile = NULL;
#endif
  if (fFile > 0)
    close(fFile);
  fFile = -1;
  fFilename = "";
}

// end
