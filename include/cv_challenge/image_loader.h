#pragma once

#include <experimental/filesystem>
#include <list>
#include <opencv2/core.hpp>

#include "cv_challenge/label.h"

namespace cv_challenge {
class ImageLoader {
public:
  ImageLoader(const std::string &folders_path);
  bool getNextImage(cv::Mat &image, cv::Mat &label);
  bool saveImage(const cv::Mat &image, const std::string &postfix = "") const;
  bool saveImage(const cv::Mat &image, Label label,
                 const std::string &postfix = "") const;

private:
  std::string images_path_;
  std::string labels_path_;
  std::string masked_path_;
  std::experimental::filesystem::path current_filename_;
  std::list<std::experimental::filesystem::path> image_filenames_;
};
}
