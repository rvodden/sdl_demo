#ifndef VISITOR_PATTERN_LEGACY_SDK_IMPL_H
#define VISITOR_PATTERN_LEGACY_SDK_IMPL_H

#include "visitor_pattern_legacy_sdk.h"
#include "legacy_sdk.h"

class BaseConverter {
  public:
    virtual ~BaseConverter() = default;
};

template<class EventClass>
class Converter {
  public:
    virtual ~Converter() = default;
    virtual OLD_Event* convert(const EventClass& event) const = 0;
};

template<class EventClass>
OLD_Event* castConverter(const EventClass& event, const BaseConverter& baseConverter){
  try {
    const Converter<EventClass> &eventConverter = dynamic_cast<const Converter<EventClass>&>(baseConverter);
    return eventConverter.convert(event);
  } catch (std::bad_cast &e) { return nullptr; } // bad cast just means this handler can't handle this event
}

class EventConverter;

class BaseEventImpl {
  friend EventConverter;
  public:
    BaseEventImpl(BaseEvent* baseEvent) : _baseEvent { baseEvent } = default;
    virtual ~BaseEventImpl() = default;
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const = 0;
  protected:
    BaseEvent* _baseEvent;
};


class UserEventImpl : public BaseEventImpl {
  using BaseEventImpl::BaseEventImpl;
  friend EventConverter;
  public:
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const { 
      OLD_Event* event = castConverter(*this, baseConverter);
      return event;
    };
};

class SystemEventImpl : public BaseEventImpl {
  using BaseEventImpl::BaseEventImpl;
  friend EventConverter;
  public:
    SystemEventImpl(std::unique_ptr<SystemEventImpl, void(*) (SystemEventImpl *)>& impl);
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const { 
      OLD_Event* event = castConverter(*this, baseConverter);
      return event;
    };
};

class CustomEventImpl : public BaseEventImpl {
  using BaseEventImpl::BaseEventImpl;
  friend EventConverter;
  public:
    CustomEventImpl(std::unique_ptr<CustomEventImpl, void(*) (CustomEventImpl *)>& impl);
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const { 
      OLD_Event* event = castConverter(*this, baseConverter);
      return event;
    };
    CustomEventImpl* clone() const;
};

class EventConverter : public BaseConverter, 
  public Converter<UserEventImpl>, 
  public Converter<SystemEventImpl>,
  public Converter<CustomEventImpl> {
  public:
   virtual OLD_Event* convert(const UserEventImpl& event) const override;
   virtual OLD_Event* convert(const SystemEventImpl& event) const override;
   virtual OLD_Event* convert(const CustomEventImpl& event) const override;
};

#endif
