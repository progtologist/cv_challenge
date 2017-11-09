#pragma once

#include <opencv2/core.hpp>

namespace cv_challenge {

typedef std::vector<cv::Point> Contour;

class ColorBucket {
public:
  void init(const cv::Mat &image, const std::vector<Contour> &contours,
            const std::vector<cv::Vec4i> &hierarchy, int index);

  bool isHuman() const;

  bool compactable(const ColorBucket &bucket);

  void addBucket(const ColorBucket &bucket);

  Contour getLarger() const;

  double getColor() const;

  void getColorWithDeviation(double &color, double &deviation) const;

  int getArea() const;

  cv::Mat getMaskedImage() const;

  cv::Mat getTransparentImage() const;

  cv::Rect getEnclosedHuman() const;
private:
  static bool getAllowedColorDiff(const double &c_a, const double &d_a,
                                    const double &c_b, const double &d_b);

  static double getDistance(const Contour &a,
                            const Contour &b);

  cv::Mat image_;
  cv::Mat mask_;
  int larger_contour_;
  std::vector<Contour> contours_;
  std::vector<int> areas_;
};
}
