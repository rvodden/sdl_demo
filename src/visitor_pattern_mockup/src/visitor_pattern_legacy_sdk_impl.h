#ifndef __VISITOR_PATTERN_LEGACY_SDK_IMPL_H__
#define __VISITOR_PATTERN_LEGACY_SDK_IMPL_H__

#include "visitor_pattern_legacy_sdk.h"
#include "legacy_sdk.h"

class BaseConverter {
  public:
    virtual ~BaseConverter() {};
};

template<class EventClass>
class Converter {
  public:
    virtual ~Converter() {};
    virtual OLD_Event* convert(const EventClass& event) const = 0;
};

class BaseConvertable {
  friend EventConverter;
  friend BaseEvent;
  public:
    BaseConvertable(const BaseEvent* baseEvent) : _baseEvent { baseEvent } {};
    virtual ~BaseConvertable() noexcept;
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const = 0;
    virtual BaseConvertable* clone(const BaseEvent*) const = 0;
    void setParent(BaseEvent* _baseEvent) const;
  protected:
    mutable const BaseEvent* _baseEvent;
};

template<class ConvertableEventClass>
class Convertable : public BaseConvertable {
  public:
    using BaseConvertable::BaseConvertable;
  
    virtual OLD_Event* acceptConverter(const BaseConverter& baseConverter) const override {
      try {
        const Converter<ConvertableEventClass> &eventConverter = dynamic_cast<const Converter<ConvertableEventClass>&>(baseConverter);
        return eventConverter.convert(*static_cast<const ConvertableEventClass*>(this));
      } catch (std::bad_cast &e) { return nullptr; } // bad cast just means this handler can't handle this event
    }

    virtual BaseConvertable* clone(const BaseEvent* parent) const {
      return new ConvertableEventClass(parent);
    }
};

class ConvertableUserEvent : public Convertable<ConvertableUserEvent> {
  using Convertable<ConvertableUserEvent>::Convertable;
};

class ConvertableSystemEvent : public Convertable<ConvertableSystemEvent> {
  using Convertable<ConvertableSystemEvent>::Convertable;
};

class ConvertableCustomEvent : public Convertable<ConvertableCustomEvent> {
  using Convertable<ConvertableCustomEvent>::Convertable;
};

class UserEventFactory {
  public:
    static UserEvent& createUserEvent(OLD_Event* event);
};

class SystemEventFactory {
  public:
    static SystemEvent& createSystemEvent(OLD_Event* event);
};

class CustomEventFactory {
  public:
    static BaseEvent& createCustomEvent(OLD_Event* event);
};

class EventConverter : public BaseConverter, 
  public Converter<ConvertableUserEvent>, 
  public Converter<ConvertableSystemEvent>, 
  public Converter<ConvertableCustomEvent> 
{
  public:
    virtual OLD_Event* convert(const ConvertableUserEvent& event) const override;
    virtual OLD_Event* convert(const ConvertableSystemEvent& event) const override;
    virtual OLD_Event* convert(const ConvertableCustomEvent& event) const override;
};

#endif
