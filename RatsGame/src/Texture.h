#pragma once

class Texture
{
public:
	Texture(const std::string& path);
	Texture(const Texture&) = default;
	Texture(Texture&&) = default;
	~Texture() {}

	void onClean();

	void bind(unsigned int slot = 0) const;
	void unbind() const;

	inline GLenum getRendererID() const { return m_rendererID; }
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }

private:
	GLenum m_rendererID;
	int m_width, m_height, m_channels;
};

