#include "cv_challenge/bucket_manager.h"
#include <opencv2/imgproc.hpp>

using namespace cv_challenge;

BucketManager::BucketManager(const cv::Mat &image, const cv::Mat &label)
    : rng_(12345) {
  original_image_ = image.clone();
  mask_aplier_.getMask(label, label_mask_);
  image.copyTo(image_, label_mask_);
  // Post-process to remove some noise
  cv::medianBlur(image_, image_, 3);
  // Add "fake" borders to close all edges at the borders
  cv::copyMakeBorder(image_, image_, 1, 1, 1, 1, cv::BORDER_CONSTANT, 0);
}

void BucketManager::createBuckets() {
  // Get the edges
  std::vector<Contour> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::Mat canny_out;
  cv::Canny(image_, canny_out, 10, 30, 3);
  cv::dilate(canny_out, canny_out, cv::Mat());
  cv::findContours(canny_out, contours, hierarchy, CV_RETR_CCOMP,
                   CV_CHAIN_APPROX_SIMPLE);
  for (size_t i = 0; i < contours.size(); ++i) {
    ColorBucket bucket;
    bucket.init(image_, contours, hierarchy, i);
    int area = bucket.getArea();
    double color, dev;
    bucket.getColorWithDeviation(color, dev);
    if (bucket.getArea() > 50 && dev < 15. && color > 0.)
      buckets_.push_back(bucket);
  }
}

void BucketManager::compact() {
  // Sort all by color
  buckets_.sort([](const ColorBucket &a, const ColorBucket &b) {
    double c_a, c_b, tmp;
    a.getColorWithDeviation(c_a, tmp);
    b.getColorWithDeviation(c_b, tmp);
    return c_a < c_b;
  });

  // Compact them
  auto iter = buckets_.begin();
  auto next_iter = std::next(iter);
  while (iter != buckets_.end()) {
    if (next_iter == buckets_.end()) {
      iter++;
      if (iter == buckets_.end())
        break;
      next_iter = std::next(iter);
      if (next_iter == buckets_.end())
        break;
    }

    if (iter->compactable(*next_iter)) {
      // Compact the two buckets
      iter->addBucket(*next_iter);
      // Remove the bucket and retry
      next_iter = buckets_.erase(next_iter);
    } else {
      if (std::fabs(iter->getColor() - next_iter->getColor()) > 10) {
        iter++; // Nothing to search after that, distance too big
        next_iter = std::next(iter);
      } else {
        next_iter++;
      }
    }
  }
}

int BucketManager::getHumans() {
  int humans = 0;
  for (const auto &bucket : buckets_) {
    if (bucket.isHuman())
      humans++;
  }
  return humans;
}

void BucketManager::drawOutput(const ImageLoader &loader) {
  for (const auto &bucket : buckets_) {
    if (bucket.isHuman()) {
      cv::Scalar color = cv::Scalar(rng_.uniform(0, 255), rng_.uniform(0, 255),
                                    rng_.uniform(0, 255));
      cv::Rect rect = bucket.getEnclosedHuman();
      cv::rectangle(original_image_, rect.tl(), rect.br(), color, 2, 8);
    }
  }
  loader.saveImage(original_image_);
}

void BucketManager::saveImages(const ImageLoader &loader) {
  int i = 0;
  for (const auto &bucket : buckets_) {
    //    if (bucket.isHuman())
    loader.saveImage(bucket.getTransparentImage(), "_" + std::to_string(i++));
  }
}
