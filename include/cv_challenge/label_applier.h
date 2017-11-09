#pragma once

#include "cv_challenge/label.h"
#include <opencv2/core.hpp>

namespace cv_challenge {

class LabelApplier {
public:
  LabelApplier();

  void getMask(const cv::Mat &input, const Label &label, cv::Mat &output) const;

  void getMask(const cv::Mat &input, cv::Mat &output) const;

  void getMaskedImage(const cv::Mat &image, const cv::Mat &label_img,
                      const Label &label, cv::Mat &output) const;

  void getMaskedImage(const cv::Mat &image, const cv::Mat &label_img,
                      cv::Mat &output) const;

private:
  const cv::Mat &getLUT(const Label &label) const;

  cv::Mat female_lut_;
  cv::Mat male_lut_;
  cv::Mat human_lut_;
};
}
