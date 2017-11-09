#include "cv_challenge/image_loader.h"
#include <iostream>
#include <opencv2/highgui.hpp>

using namespace cv_challenge;
using namespace std::experimental::filesystem;

ImageLoader::ImageLoader(const std::string &folders_path) {
  images_path_ = folders_path + "/images";
  labels_path_ = folders_path + "/labels";
  masked_path_ = folders_path + "/masked";
  if (exists(path(masked_path_))) {
    remove_all(path(masked_path_));
  }
  if (!create_directory(path(masked_path_))) {
    std::string error = "Failed to create masked directory";
    std::cerr << error << std::endl;
    throw std::runtime_error(error);
  }
  for (auto &file : directory_iterator(images_path_)) {
    image_filenames_.push_back(file.path().filename());
  }
}

bool ImageLoader::getNextImage(cv::Mat &image, cv::Mat &label) {
  if (!image_filenames_.empty()) {
    current_filename_ = image_filenames_.front();
    path image_path = images_path_;
    image_path /= current_filename_;
    path label_path = labels_path_;
    label_path /= current_filename_;
    image = cv::imread(image_path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    if (!image.data) {
      throw std::runtime_error("Could not load image from file " +
                               std::string(image_path.c_str()));
    }
    label = cv::imread(label_path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    if (!label.data) {
      throw std::runtime_error("Could not load label from file " +
                               std::string(label_path.c_str()));
    }
    image_filenames_.pop_front();
    return true;
  }
  return false;
}

bool ImageLoader::saveImage(const cv::Mat &image, const std::string &postfix) const {
  if (!current_filename_.empty()) {
    path masked_path = masked_path_;
    masked_path /= current_filename_.stem();
    masked_path += postfix;
    masked_path += current_filename_.extension();
    cv::imwrite(masked_path.c_str(), image);
    return true;
  }
  return false;
}

bool ImageLoader::saveImage(const cv::Mat &image, Label label,
                            const std::string &postfix) const {
  if (!current_filename_.empty()) {
    path masked_path = masked_path_;
    masked_path /= current_filename_.stem();
    masked_path += "_" + to_string(label) + postfix;
    masked_path += current_filename_.extension();
    cv::imwrite(masked_path.c_str(), image);
    return true;
  }
  return false;
}
