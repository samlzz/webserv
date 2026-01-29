// ICgiOutputSink.hpp
#ifndef __IOUTPUT_SINK_HPP__
# define __IOUTPUT_SINK_HPP__

# include <cstddef>

class IOutputSink {
public:
	virtual ~IOutputSink() {}

	virtual void	append(const char* data, size_t n) = 0;
	virtual void	finalize(void) = 0;
};

#endif /* __IOUTPUT_SINK_HPP__ */