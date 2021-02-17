#include <QDir>
#include <QString>
#include <iostream>

#include "config.h"
#include "ocr.h"
#include "utils.h"

OCR::OCR() {
    tess = new tesseract::TessBaseAPI();
    orientation = NONE;
}

OCR::~OCR() {
    tess->End();
    delete tess;
    delete [] result;
    pixDestroy(&image);
}

PIX *OCR::processImage(QString path) {
    float factor = 3.5f;
    const int otsuSX = 2000;
    const int otsuSY = 2000;
    const int otsuSmoothX = 0;
    const int otsuSmoothY = 0;
    const float otsuScorefract = 0.0f;

    const int usmHalfwidth = 5;
    const float usmFract = 2.5f;

    QByteArray array = path.toUtf8();
    const char *imageLocation = array.constData();
    PIX *pixs = pixRead(imageLocation);
    if (pixGetDepth(pixs) == 8){
      // Already Grayscale so no conversion required.
      ;
      // FIXME Find a better solution for this, Couldn't find how to
      // check for number of bands with pixmap
    }else{
      // Convert to grayscale
      pixs = pixConvertRGBToGray(pixs, 0.0, 0.0, 0.0);
    }

    // Resize
    pixs = pixScale(pixs, factor, factor);

    pixs = pixUnsharpMaskingGray(pixs, usmHalfwidth, usmFract);
    pixOtsuAdaptiveThreshold(pixs, otsuSX, otsuSY, otsuSmoothX, otsuSmoothY,
            otsuScorefract, nullptr, &pixs);
    pixs = pixSelectBySize(pixs, 3, 3, 8, L_SELECT_IF_EITHER, L_SELECT_IF_GT,
            nullptr);

    // Decide if image needs to be inverted or not
    float pixelAvg = pixAverageOnLine(pixs, 0, 0, pixs->w - 1, 0, 1);
    pixelAvg +=
        pixAverageOnLine(pixs, 0, pixs->h - 1, pixs->w - 1, pixs->h - 1, 1);
    pixelAvg += pixAverageOnLine(pixs, 0, 0, 0, pixs->h - 1, 1);
    pixelAvg +=
        pixAverageOnLine(pixs, pixs->w - 1, 0, pixs->w - 1, pixs->h - 1, 1);
    pixelAvg /= 4.0f;

    if (pixelAvg > .7) {
        pixs = pixInvert(pixs, pixs);
    }

    // Add a border
    pixs = pixAddBlackOrWhiteBorder(pixs, 10, 10, 10, 10, L_GET_WHITE_VAL);

#ifdef DEBUG
    QString tempImgDebug = "/tmp/tempImgDebug.png";
    QByteArray tempArrayDebug = tempImgDebug.toUtf8();
    const char *imgDebugLocation = tempArrayDebug.constData();

    pixWrite(imgDebugLocation, pixs, IFF_PNG);
#endif
    return pixs;
}

void OCR::extractText() {
    tess->SetImage(image);
    result = tess->GetUTF8Text();
}

char *OCR::ocrImage(QString path, ORIENTATION orn) {
    if (orn != orientation) {
        this->setLanguage(orn);
    }

    image = processImage(path);
    extractText();
    remove_spaces(result);
    return result;
}

void OCR::setLanguage(ORIENTATION orn) {
    QString lang = "jpn";

    if (orn == VERTICAL) {
        lang += "_vert";
    }

    tess->Init(GAZOU_MODEL_FOLDER, lang.toLocal8Bit().constData(),
                tesseract::OEM_LSTM_ONLY);

    this->setJapaneseParams();

    tesseract::PageSegMode pageSeg = orn == VERTICAL
        ? tesseract::PSM_SINGLE_BLOCK_VERT_TEXT
        : tesseract::PSM_SINGLE_BLOCK;

    tess->SetPageSegMode(pageSeg);

    orientation = orn;
}

void OCR::setJapaneseParams() {
    tess->SetVariable("tessedit_char_blacklist", "}><L");
    tess->SetVariable("textord_debug_tabfind", "0");
    tess->SetVariable("language_model_ngram_on", "0");
    tess->SetVariable("tessedit_enable_dict_correction", "1");
    tess->SetVariable("textord_really_old_xheight", "1");
    tess->SetVariable("textord_force_make_prop_words", "F");
    tess->SetVariable("edges_max_children_per_outline", "40");
    tess->SetVariable("tosp_threshold_bias2", "1");
    tess->SetVariable("classify_norm_adj_midpointt", "96");
    tess->SetVariable("tessedit_class_miss_scale", "0.002");
    tess->SetVariable("textord_initialx_ile", "1.0");
    tess->SetVariable("preserve_interword_spaces", "1");
    tess->SetVariable("user_defined_dpi", "300");
    tess->SetVariable("debug_file", "/dev/null"); 
}
