#include "cv_challenge/bucket_manager.h"
#include "cv_challenge/config.h"
#include "cv_challenge/image_loader.h"
#include <iostream>

int main(int /*argc*/, char ** /*argv*/) {
  using namespace cv_challenge;
  ImageLoader loader(IMAGES_PATH);

  cv::Mat image, label, mask, out;
  size_t i = 0;
  std::chrono::milliseconds total_duration = std::chrono::milliseconds(0);
  while (loader.getNextImage(image, label)) {
    auto start = std::chrono::high_resolution_clock::now();
    BucketManager manager(image, label);
    manager.createBuckets();
    manager.compact();
    int humans = manager.getHumans();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    total_duration +=
        std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "Found " << humans << " humans in "
              << std::to_string(duration.count() / 1000000.) << " msec" << std::endl;
    manager.drawOutput(loader);
    i++;
  }

  std::cout << "Processing all took " << total_duration.count()
            << " milliseconds" << std::endl;

  std::cout << "Average processing per image "
            << std::to_string(static_cast<double>(total_duration.count()) / i)
            << " milliseconds" << std::endl;

  std::cout << "Processed " << i << " images" << std::endl;
  return 0;
}
