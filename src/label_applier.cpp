#include "cv_challenge/label_applier.h"

#include <opencv2/imgproc.hpp>

using namespace cv_challenge;

LabelApplier::LabelApplier() {
  // Could be static (variables and methods) but OpenCV segfaults
  // http://answers.opencv.org/question/4072/initializing-static-cvmat-with-cvmatzeros-causes-segmentation-fault/
  // https://github.com/opencv/opencv/issues/5174

  male_lut_ = cv::Mat::zeros(1, 256, CV_8U);
  male_lut_.data[4] = 255;
  female_lut_ = cv::Mat::zeros(1, 256, CV_8U);
  female_lut_.data[3] = 255;
  human_lut_ = cv::Mat::zeros(1, 256, CV_8U);
  human_lut_.data[3] = 255;
  human_lut_.data[4] = 255;
}

void LabelApplier::getMask(const cv::Mat &input, const Label &label,
                           cv::Mat &output) const {
  cv::LUT(input, getLUT(label), output);
}

void LabelApplier::getMask(const cv::Mat &input, cv::Mat &output) const {
  cv::LUT(input, human_lut_, output);
}

void LabelApplier::getMaskedImage(const cv::Mat &image,
                                  const cv::Mat &label_img, const Label &label,
                                  cv::Mat &output) const {
  cv::Mat label_lut;
  getMask(label_img, label, label_lut);
  image.copyTo(output, label_lut);
}

void LabelApplier::getMaskedImage(const cv::Mat &image,
                                  const cv::Mat &label_img,
                                  cv::Mat &output) const {
  cv::Mat label_lut;
  getMask(label_img, label_lut);
  image.copyTo(output, label_lut);
  std::vector<cv::Mat> channels;
  channels.push_back(output);    // B
  channels.push_back(output);    // G
  channels.push_back(output);    // R
  channels.push_back(label_lut); // A
  cv::merge(channels, output);
}

const cv::Mat &LabelApplier::getLUT(const Label &label) const {
  switch (label) {
  case Label::female:
    return female_lut_;
  case Label::male:
    return male_lut_;
  default:
    throw std::runtime_error("Look Up Tables for this label are not supported");
  }
}
