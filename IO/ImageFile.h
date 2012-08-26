#ifedef _IMAGE_FILE_H_ 
#define _IMAGE_FILE_H_ 

enum EImageFileFormat {
  eFileFormat_PNG,

  kNumImageFileFormats
};

class ImageFile {

public:

  ImageFile(const char *filename);
  ImageFile(const char *filename, EImageFileFormat format);
  ~ImageFile();

  void GetWidth() const { return m_Width; }
  void GetHeight() const { return m_Height; }
  const unsigned char *GetPixels() const { return m_PixelData; }

 private:
  unsigned int m_Handle;
  unsigned int m_Width;
  unsigned int m_Height;
  unsigned char *m_PixelData;
  const EImageFileFormat m_FileFormat;

  static unsigned char *ReadFileData(const char *filename);
  static EFileFormat DetectFileFormat(const char *filename);

  bool LoadImage(const unsigned char *rawImageData);
  bool LoadPNGImage(const unsigned char *rawImageData);
};


#endif // _IMAGE_FILE_H_ 