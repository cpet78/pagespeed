/*
 * Copyright 2010 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: jmaessen@google.com (Jan Maessen)

#ifndef NET_INSTAWEB_REWRITER_PUBLIC_IMAGE_H_
#define NET_INSTAWEB_REWRITER_PUBLIC_IMAGE_H_

#include <cstddef>

#include "net/instaweb/util/public/basictypes.h"
#include "net/instaweb/util/public/string.h"
#include "net/instaweb/util/public/string_util.h"

namespace net_instaweb {
class ImageDim;
class MessageHandler;
struct ContentType;

class Image {
 public:
  // Images that are in the process of being transformed are represented by an
  // Image.  This class encapsulates various operations that are sensitive to
  // the format of the compressed image file and of the image libraries we are
  // using.  In particular, the timing of compression and decompression
  // operations may be a bit unexpected, because we may do these operations
  // early in order to retrieve image metadata, or we may choose to skip them
  // entirely if we don't need them or don't understand how to do them.
  //
  // In future we may need to plumb this to other data sources or change how
  // metadata is retrieved; the object is to do so locally in this class without
  // disrupting any of its clients.

  enum Type {
    IMAGE_UNKNOWN = 0,
    IMAGE_JPEG,
    IMAGE_PNG,
    IMAGE_GIF,
  };

  virtual ~Image();

  // Stores the image dimensions in natural_dim (on success, sets
  // natural_dim->{width, height} and
  // ImageUrlEncoder::HasValidDimensions(natural_dim) == true).  This
  // method can fail (ImageUrlEncoder::HasValidDimensions(natural_dim)
  // == false) for various reasons: we don't understand the image
  // format, we can't find the headers, the library doesn't support a
  // particular encoding, etc.  In that case the other fields are left
  // alone.
  virtual void Dimensions(ImageDim* natural_dim) = 0;

  // Returns the size of original input in bytes.
  size_t input_size() const {
    return original_contents_.size();
  }

  // Returns the size of output image in bytes.
  size_t output_size() {
    size_t ret;
    if (output_valid_ || ComputeOutputContents()) {
      ret = output_contents_.size();
    } else {
      ret = input_size();
    }
    return ret;
  }

  Type image_type() {
    if (image_type_ == IMAGE_UNKNOWN) {
      ComputeImageType();
    }
    return image_type_;
  }

  // Changes the size of the image to the given width and height.  This will run
  // image processing on the image, and return false if the image processing
  // fails.  Otherwise the image contents and type can change.
  virtual bool ResizeTo(const ImageDim& new_dim) = 0;

  // Returns image-appropriate content type, or NULL if no content type is
  // known.  Result is a top-level const pointer and should not be deleted etc.
  const ContentType* content_type();

  // Returns the best known image contents.  If image type is not understood,
  // then Contents() will have NULL data().
  StringPiece Contents();

  // Draws the given image on top of this one at the given offset.  Returns true
  // if successful.
  virtual bool DrawImage(Image* image, int x, int y) = 0;

  // Attempts to decode this image and load its raster into memory.  If this
  // returns false, future calls to DrawImage and ResizeTo will fail.
  virtual bool EnsureLoaded() = 0;

 protected:
  explicit Image(const StringPiece& original_contents);
  explicit Image(Type type);

  // Internal helpers
  virtual void ComputeImageType() = 0;
  virtual bool ComputeOutputContents() = 0;

  Type image_type_;  // Lazily initialized, initially IMAGE_UNKNOWN.
  const StringPiece original_contents_;
  GoogleString output_contents_;  // Lazily filled.
  bool output_valid_;             // Indicates output_contents_ now correct.

 private:
  DISALLOW_COPY_AND_ASSIGN(Image);
};

// Image owns none of its inputs.  All of the arguments to Image(...) (the
// original_contents in particular) must outlive the Image object itself.  The
// intent is that an Image is created in a scoped fashion from an existing
// known resource.
Image* NewImage(const StringPiece& original_contents,
                const GoogleString& url,
                const StringPiece& file_prefix,
                MessageHandler* handler);

// Creates a blank image of the given dimensions and type.
// For now, this is assumed to be an 8-bit 3-channel image.
Image* BlankImage(int width, int height, Image::Type type,
                  const StringPiece& tmp_dir, MessageHandler* handler);

}  // namespace net_instaweb

#endif  // NET_INSTAWEB_REWRITER_PUBLIC_IMAGE_H_