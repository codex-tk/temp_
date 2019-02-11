/**
 * @file memory_buffer_sink.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __tlab_ext_live555_memory_buffer_sink_h__
#define __tlab_ext_live555_memory_buffer_sink_h__

#include <tlab/internal.hpp>
#include <tlab/ext/live555/envir_loop.hpp>
#include <tlab/basic_buffer.hpp>

#include "MediaSink.hh"

namespace tlab::ext::live555{

class memory_buffer_sink : public MediaSink {
public:
    static memory_buffer_sink *createNew(envir_loop &env/*,
                                       unsigned int bufferSize,
                                       unsigned int sliceDuration*/) {
        return new memory_buffer_sink(env, 0,0);//bufferSize, sliceDuration);
    }

protected:
    memory_buffer_sink(envir_loop &env, unsigned bufferSize,
                     unsigned int sliceDuration);
    virtual ~memory_buffer_sink();

    virtual Boolean continuePlaying();

    static void afterGettingFrame(void *clientData, unsigned frameSize,
                                  unsigned numTruncatedBytes,
                                  struct timeval presentationTime,
                                  unsigned durationInMicroseconds) {
        memory_buffer_sink *sink = (memory_buffer_sink *)clientData;
        sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
    }

    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                           struct timeval presentationTime);
/*
public:
    unsigned int getBufferSize(unsigned int slice);
    std::string getBuffer(unsigned int slice);
    unsigned int firstTime();
    unsigned int duration();
    unsigned int getSliceDuration() { return m_sliceDuration; }
private:
    unsigned char *m_buffer;
    unsigned int m_bufferSize;
    std::map<unsigned int, std::string> m_outputBuffers;
    unsigned int m_refTime;
    unsigned int m_sliceDuration;
    */
    tlab::byte_buffer _frame_buffer;
    tlab::byte_buffer _buffer;
    int _index;
    long _sec;
};

memory_buffer_sink::memory_buffer_sink(envir_loop& env, unsigned bufferSize, unsigned int sliceDuration) 
    : MediaSink(env.envir())//, m_bufferSize(bufferSize), m_refTime(0), m_sliceDuration(sliceDuration)
{
    _index = 1;
    _sec = 0;
    _frame_buffer.reserve(1024768);
	//m_buffer = new unsigned char[m_bufferSize];
}

memory_buffer_sink::~memory_buffer_sink() 
{
	//delete[] m_buffer;
}


Boolean memory_buffer_sink::continuePlaying() 
{
	Boolean ret = False;
	if (fSource != NULL) 
	{
		fSource->getNextFrame( reinterpret_cast<unsigned char*>(_frame_buffer.pptr()), _frame_buffer.psize(),
				afterGettingFrame, this,
				onSourceClosure, this);
		ret = True;
	}
	return ret;
}


void memory_buffer_sink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime) 
{
	if (numTruncatedBytes > 0) 
	{
        /*
		envir() << "FileSink::afterGettingFrame(): The input frame data was too large for our buffer size \n";
		// realloc a bigger buffer
		m_bufferSize += numTruncatedBytes;
		delete[] m_buffer;
		m_buffer = new unsigned char[m_bufferSize];
        */
       _frame_buffer.reserve(_frame_buffer.size() + numTruncatedBytes);
	}
	else
	{			
		// append buffer to slice buffer
        /*
		if (m_refTime == 0)
		{
			m_refTime = presentationTime.tv_sec;
		}
		unsigned int slice = (presentationTime.tv_sec-m_refTime)/m_sliceDuration;
		std::string& outputBuffer = m_outputBuffers[slice];
		outputBuffer.append((const char*)m_buffer, frameSize);
		
		// remove old buffers
		while (m_outputBuffers.size()>3)
		{
			m_outputBuffers.erase(m_outputBuffers.begin());
		}*/
        if(_sec == 0 ){
            _sec = presentationTime.tv_sec;
        }
        if (presentationTime.tv_sec - _sec > 10)
        {
            _sec = presentationTime.tv_sec;
            char fname[1024];
            sprintf(fname, "%04d.ts" , _index);
            FILE* fp = fopen(fname,"wb");
            fwrite( _buffer.gptr() , 1 , _buffer.gsize() , fp);
            fclose(fp);
            _buffer.clear();
            ++_index;
        }
        _frame_buffer.pptr(frameSize);
        _buffer.reserve(_frame_buffer.gsize());
        memcpy(_buffer.pptr(),_frame_buffer.gptr(),_frame_buffer.gsize());
        _buffer.pptr(_frame_buffer.gsize());
        _frame_buffer.clear();
	}

	continuePlaying();
}
/*
unsigned int memory_buffer_sink::getBufferSize(unsigned int slice)
{
	unsigned int size = 0;
	std::map<unsigned int,std::string>::iterator it = m_outputBuffers.find(slice);
	if (it != m_outputBuffers.end())
	{
		size = it->second.size();
	}
	return size;
}

std::string memory_buffer_sink::getBuffer(unsigned int slice)
{
	std::string content;
	std::map<unsigned int,std::string>::iterator it = m_outputBuffers.find(slice);
	if (it != m_outputBuffers.end())
	{
		content = it->second;

	}
	return content;
}

unsigned int memory_buffer_sink::firstTime()
{
	unsigned int firstTime = 0;
	if (m_outputBuffers.size() != 0)				
	{
		firstTime = m_outputBuffers.begin()->first;
	}
	return firstTime*m_sliceDuration;
}

unsigned int memory_buffer_sink::duration()
{
	unsigned int duration = 0;
	if (m_outputBuffers.size() != 0)				
	{
		duration = m_outputBuffers.rbegin()->first - m_outputBuffers.begin()->first;
	}
	return (duration)*m_sliceDuration;
}*/
}

#endif