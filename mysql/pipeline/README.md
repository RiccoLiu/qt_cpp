# 编译&运行&测试

# uitls::Image 

## 图像金字塔

## 图像格式转换

```
opencv
```

## 图像保存


- 图像编码保存
```
utils::Image<unsigned char> img(width, height);
...

cv::Mat cv_img(img.Height(), img.Width(), CV_8UC1, img.Data());
cv::imwrite("img.png", cv_img);
```

- 保存单通道图像
```

utils::Image<unsigned char> img(width, height);
....

std::ofstream file("image.raw", std::ios::binary);
file.write(reinterpret_cast<const char*>(img.data()),
           img.rows() * img.cols() * sizeof(char));
file.close();
```

- 读取单通道图像

```
Image<unsigned char> img(width, height);
std::ifstream file(filename, std::ios::binary);
file.read(reinterpret_cast<char*>(img.data()), width * height);
file.close();
```
