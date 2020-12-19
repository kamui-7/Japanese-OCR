#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

enum ORIENTATION
{
    HORIZONTAL,
    VERTICAL
};

class OCR
{
public:
    OCR();
    ~OCR();
    char *ocrImage(char const *path, ORIENTATION orn);

private:
    tesseract::TessBaseAPI *tess;
    char *result;

    PIX *processImage(char const *path);
    void extractText(PIX *img);
    void setLanguage(ORIENTATION orn = VERTICAL);
    void setJapaneseParams();
    void postProcessText();
};