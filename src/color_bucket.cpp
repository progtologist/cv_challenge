#include "cv_challenge/color_bucket.h"

#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <sstream>

using namespace cv_challenge;

void ColorBucket::init(const cv::Mat &image,
                       const std::vector<Contour> &contours,
                       const std::vector<cv::Vec4i> &hierarchy, int index) {
  image_ = image;
  cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
  // Draw the entire contour
  cv::drawContours(mask, contours, index, cv::Scalar(255), CV_FILLED);
  // And then remove all the holes
  int hole = hierarchy[index][2]; // Get first hole
  int removable_area = 0;
  while (hole != -1) {
    cv::drawContours(mask, contours, hole, cv::Scalar(0), CV_FILLED);
    removable_area += static_cast<int>(cv::contourArea(contours[hole]));
    hole = hierarchy[hole][0]; // Get next hole
  }
  mask_ = mask;
  contours_.push_back(contours[index]);
  int area =
      static_cast<int>(cv::contourArea(contours[index])) - removable_area;
  auto it = std::max_element(areas_.begin(), areas_.end());
  if (it == areas_.end())
    larger_contour_ = contours_.size() - 1;
  else if (area > *it) {
    larger_contour_ = contours_.size() - 1;
  }
  areas_.push_back(area);
}

bool ColorBucket::isHuman() const {
  double c = getColor();
  int a = getArea();
  if (c > 100) {
    if (a > 500)
      return true;
    return false;
  }
  if (c > 70) {
    if (a > 1000)
      return true;
    return false;
  }
  if (c > 50) {
    if (a > 2000)
      return true;
    return false;
  }
  if (c > 1) {
    if (a > 3000)
      return true;
    return false;
  }
  return false;
}

bool ColorBucket::compactable(const ColorBucket &bucket) {
  // First compare the colors
  double c1, c2, d1, d2;
  getColorWithDeviation(c1, d1);
  bucket.getColorWithDeviation(c2, d2);
  if (!getAllowedColorDiff(c1, d1, c2, d2))
    return false;

  // If the colors match, check the distance
  Contour a = getLarger();
  Contour b = bucket.getLarger();
  if (getDistance(a, b) > 50)
    return false;

  if (isHuman() && bucket.isHuman())
    return false;

  return true;
}

void ColorBucket::addBucket(const ColorBucket &bucket) {
  // Unite masks
  mask_ |= bucket.mask_;
  size_t previous_size = contours_.size();
  contours_.insert(contours_.end(), bucket.contours_.begin(),
                   bucket.contours_.end());
  int parent_area = static_cast<int>(cv::contourArea(getLarger()));
  int bucket_area = static_cast<int>(cv::contourArea(bucket.getLarger()));
  if (bucket_area > parent_area)
    larger_contour_ = previous_size + bucket.larger_contour_;
  areas_.insert(areas_.end(), bucket.areas_.begin(), bucket.areas_.end());
}

Contour ColorBucket::getLarger() const { return contours_[larger_contour_]; }

double ColorBucket::getColor() const {
  cv::Scalar mean;
  mean = cv::mean(image_, mask_);
  return mean[0];
}

void ColorBucket::getColorWithDeviation(double &color,
                                        double &deviation) const {
  cv::Scalar mean, dev;
  cv::meanStdDev(image_, mean, dev, mask_);
  color = mean[0];
  deviation = dev[0];
}

int ColorBucket::getArea() const {
  return std::accumulate(areas_.begin(), areas_.end(), 0);
}

cv::Mat ColorBucket::getMaskedImage() const {
  cv::Mat out;
  image_.copyTo(out, mask_);
  return out;
}

cv::Mat cv_challenge::ColorBucket::getTransparentImage() const {
  cv::Mat out;
  std::vector<cv::Mat> channels;
  channels.push_back(image_);
  channels.push_back(image_);
  channels.push_back(image_);
  channels.push_back(mask_);
  cv::merge(channels, out);
  // Write information on top
  double color, deviation;
  getColorWithDeviation(color, deviation);
  std::stringstream text;
  text << std::fixed << std::setprecision(4)
       << "Color: " << std::to_string(color) << " Dev: " << deviation
       << " Area: " << getArea();
  cv::putText(out, text.str(), cv::Point(30, 30),
              cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 0, 255), 1,
              CV_AA);

  return out;
}

cv::Rect ColorBucket::getEnclosedHuman() const {
  Contour total_contour;
  for (const auto &contour : contours_) {
    total_contour.insert(total_contour.end(), contour.begin(), contour.end());
  }
  cv::Rect bounding_rect = cv::boundingRect(total_contour);
  return bounding_rect;
}

bool ColorBucket::getAllowedColorDiff(const double &c_a, const double &d_a,
                                      const double &c_b, const double &d_b) {
  // Check if the color difference is sensible with respect to the color
  // deviation
  double color_difference = std::fabs(c_b - c_a);
  if (color_difference > 5)
    return false;
  double deviation_sum = d_a + d_b;
  if (color_difference > deviation_sum)
    return false;
  return true;
}

double ColorBucket::getDistance(const Contour &a, const Contour &b) {
  // Brute force is actually faster for small contours like the ones
  // we are dealing with
  double distance = std::numeric_limits<double>::max();
  for (const auto &pa : a) {
    for (const auto &pb : b) {
      cv::Point diff = pa - pb;
      double d = std::pow(diff.x, 2) + std::pow(diff.y, 2);
      if (d < distance)
        distance = d;
    }
  }
  return std::sqrt(distance);
}
