#ifndef PIXEL_H
#define PIXEL_H

#include <cstdint>
#include <variant>
#include <optional>

namespace sdlpp {

enum class PixelType : uint8_t {
  kUnknown,
  kIndex1,
  kIndex2,
  kIndex4,
  kIndex8,
  kPacked8,
  kPacked16,
  kPacked32,
  kArrayU8,
  kArrayU16,
  kArrayU32,
  kArrayF16,
  kArrayF32,
};

enum class BitmapOrder : uint8_t {
  kNone,
  k4321,
  k1234,
};

enum class PackedOrder : uint8_t {
  kNone,
  kXRGB,
  kRGBX,
  kARGB,
  kRGBA,
  kXBGR,
  kBGRX,
  kABGR,
  kBGRA,
};

enum class ArrayOrder: uint8_t {
  kNone,
  kRGB,
  kRGBA,
  kARGB,
  kBGR,
  kBGRA,
  kABGR
};

enum class PackedLayout: uint8_t {
  kNone,
  k332,
  k4444,
  k1555,
  k5551,
  k565,
  k8888,
  k2101010,
  k1010102
};

class PixelFormat {
  public:
    constexpr PixelFormat(
      PixelType type,
      BitmapOrder order,
      PackedLayout layout,
      uint8_t bits,
      uint8_t bytes
    ) : _type(type), _order(order), _layout(layout), _bits(bits), _bytes(bytes) {};

    constexpr PixelFormat(
      PixelType type,
      PackedOrder order,
      PackedLayout layout,
      uint8_t bits,
      uint8_t bytes
    ) : _type(type), _order(order), _layout(layout), _bits(bits), _bytes(bytes) {};

    constexpr PixelFormat(
      PixelType type,
      ArrayOrder order,
      PackedLayout layout,
      uint8_t bits,
      uint8_t bytes
    ) : _type(type), _order(order), _layout(layout), _bits(bits), _bytes(bytes) {};

    [[nodiscard]] constexpr auto getType() const { return _type; }
    [[nodiscard]] constexpr auto getOrder() const { return _order; }
    [[nodiscard]] constexpr auto getLayout() const { return _layout;}
    [[nodiscard]] constexpr auto getBits() const { return _bits; }
    [[nodiscard]] constexpr auto getBytes() const { return _bytes; }

    [[nodiscard]] constexpr auto getBitmapOrder() const -> std::optional<BitmapOrder> {
      if (std::holds_alternative<BitmapOrder>(_order)) {
        return std::get<BitmapOrder>(_order);
      }
      return std::nullopt;
    }

    [[nodiscard]] constexpr auto getPackedOrder() const -> std::optional<PackedOrder> {
      if (std::holds_alternative<PackedOrder>(_order)) {
        return std::get<PackedOrder>(_order);
      }
      return std::nullopt;
    }

    [[nodiscard]] constexpr auto getArrayOrder() const -> std::optional<ArrayOrder> {
      if (std::holds_alternative<ArrayOrder>(_order)) {
        return std::get<ArrayOrder>(_order);
      }
      return std::nullopt;
    }


    [[nodiscard]] constexpr auto isIndexed() const -> bool { 
      switch (getType()) {
        case PixelType::kIndex1:
        case PixelType::kIndex2:
        case PixelType::kIndex4:
        case PixelType::kIndex8:
          return true;
        default:
          return false;
      }
    }

    [[nodiscard]] constexpr auto isPacked() const -> bool {
      switch (getType()) {
        case PixelType::kPacked8:
        case PixelType::kPacked16:
        case PixelType::kPacked32:
          return true;
        default:
          return false;
      }
    }

    [[nodiscard]] constexpr auto isArray() const -> bool {
      switch (getType()) {
        case PixelType::kArrayU8:
        case PixelType::kArrayU16:
        case PixelType::kArrayU32:
        case PixelType::kArrayF16:
        case PixelType::kArrayF32:
          return true;
        default:
          return false;
      }
    }

    [[nodiscard]] constexpr auto isTenBit() const -> bool {
      return getType() == PixelType::kPacked32 || getLayout() == PackedLayout::k2101010;
    }

    [[nodiscard]] constexpr auto isFloat() const -> bool {
      return getType() == PixelType::kArrayF16 || getType() == PixelType::kArrayF32;
    }

    [[nodiscard]] constexpr auto isAlpha() const -> bool {
      if (auto packed_order = getPackedOrder()) {
        return *packed_order == PackedOrder::kARGB ||
               *packed_order == PackedOrder::kRGBA ||
               *packed_order == PackedOrder::kABGR ||
               *packed_order == PackedOrder::kBGRA;
      }
      if (auto array_order = getArrayOrder()) {
        return *array_order == ArrayOrder::kARGB ||
               *array_order == ArrayOrder::kRGBA ||
               *array_order == ArrayOrder::kABGR ||
               *array_order == ArrayOrder::kBGRA;
      }
      return false;
    }

  private:
    PixelType _type;
    std::variant<BitmapOrder, PackedOrder, ArrayOrder> _order;
    PackedLayout _layout;
    uint8_t _bits;
    uint8_t _bytes;
};



 
} // namespace sdlpp

#endif
