#pragma once

class Image {

	unsigned int _id = 0;
	int _width = 0;
	int _height = 0;
	int _channels = 0;

public:
	unsigned int id() const { return _id; }
	int width() const { return _width; }
	int height() const { return _height; }
	int channels() const { return _channels; }

	Image() = default;
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept = delete;
	~Image();

	void bind() const;
	void load(int width, int height, int channels, void* data);
};
