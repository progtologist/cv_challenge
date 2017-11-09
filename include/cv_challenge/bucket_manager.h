#pragma once

#include "cv_challenge/color_bucket.h"
#include "cv_challenge/label_applier.h"
#include "cv_challenge/image_loader.h"

namespace cv_challenge {

class BucketManager {
public:
  BucketManager(const cv::Mat &image, const cv::Mat &label);
  void createBuckets();
  void compact();
  int getHumans();
  void drawOutput(const ImageLoader& loader);
  void saveImages(const ImageLoader &loader);
private:
  LabelApplier mask_aplier_;
  cv::Mat original_image_;
  cv::Mat image_;
  cv::Mat label_mask_;
  cv::RNG rng_;
  std::list<ColorBucket> buckets_;
};

}
