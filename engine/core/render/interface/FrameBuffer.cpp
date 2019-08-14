#include "Renderer.h"
#include "interface/FrameBuffer.h"
#include "interface/ShaderProgram.h"
#include "engine/core/main/Engine.h"

namespace Echo
{
	FrameBuffer::FrameBuffer( ui32 id, ui32 width, ui32 height)
		: m_id( id)
        , m_width(width)
        , m_height(height)
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	void FrameBuffer::bindMethods()
	{

	}
}