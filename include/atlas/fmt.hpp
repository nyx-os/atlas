#pragma once
#include "cstr.hpp"
#include "string.hpp"
#include "string_view.hpp"
#include "tuple.hpp"

namespace Atlas {

template <typename T>
concept FormatSink = Container<T, char>;

struct FormatOptions {
  int base = 10;
  char padding = ' ';
  bool capital = false;
  size_t width = 0;
};

template <FormatSink S, typename T>
void format_digits(S &sink, T value, bool negative,
                   const FormatOptions options) {
  const char *digits =
      options.capital ? "0123456789ABCDEF" : "0123456789abcdef";
  char buffer[64];

  if (negative) {
    sink.push('-');
  }

  char *p = buffer + sizeof(buffer);
  *--p = '\0';

  do {
    *--p = digits[value % options.base];
    value /= options.base;
  } while (value);

  size_t len = buffer + sizeof(buffer) - p - 1;
  if (negative) {
    len++;
  }
  size_t pad = (options.width - len);

  if (len < options.width) {
    while (pad-- > 0) {
      sink.push(options.padding);
    }
  }

  for (; *p; p++) {
    sink.push(*p);
  }
}

template <typename... Ts> struct FmtImpl {
  const char *fmt;
  Tuple<Ts...> values;
};

template <typename T> struct Formatter;

template <std::signed_integral T> struct Formatter<T> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, T value) {
    if (value < 0) {
      auto absv = ~static_cast<typename std::make_unsigned_t<T>>(value) + 1;
      format_digits(sink, absv, true, opts);
    } else {
      format_digits(sink, value, false, opts);
    }
  }
};

template <std::unsigned_integral T> struct Formatter<T> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, T value) {
    format_digits(sink, value, false, opts);
  }
};

template <> struct Formatter<bool> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, bool value) {
    (void)opts;
    sink.push('#');
    if (value) {
      sink.push('t');
    } else {
      sink.push('f');
    }
  }
};

template <> struct Formatter<const char *> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, const char *value) {
    (void)opts;
    for (const char *p = value; *p; p++) {
      sink.push(*p);
    }
  }
};

template <> struct Formatter<char> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, char value) {
    (void)opts;
    sink.push(value);
  }
};

template <> struct Formatter<StringView> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, const StringView &value) {
    (void)opts;
    for (auto c : value) {
      sink.push(c);
    }
  }
};
template <> struct Formatter<String> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, const String &value) {
    (void)opts;
    for (auto c : value) {
      sink.push(c);
    }
  }
};

template <typename T> struct Formatter<T *> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, T *value) {
    opts.base = 16;
    format_digits(sink, reinterpret_cast<uintptr_t>(value), false, opts);
  }
};

template <typename T> struct Formatter<Option<T>> {
  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, Option<T> value) {
    if (value.is_some()) {
      format_value(*value, opts, sink);
    } else {
      sink.push('N');
      sink.push('o');
      sink.push('n');
      sink.push('e');
    }
  }
};

template <typename Sink, typename T>
void format_value(T value, FormatOptions opts, Sink &sink) {
  Formatter<T>{}.format(sink, opts, value);
}

template <typename... Ts> struct Formatter<FmtImpl<Ts...>> {

  template <FormatSink Sink>
    requires(sizeof...(Ts) > 0)
  auto format_nth(FmtImpl<Ts...> &fmt, size_t index, FormatOptions opts,
                  Sink &sink) {
    if (index >= sizeof...(Ts)) {
      return 0;
    }

    return [&]<size_t... I>(std::index_sequence<I...>) {
      return ((index == I ? (::Atlas::format_value(fmt.values.template get<I>(),
                                                   opts, sink),
                             1)
                          : 0) +
              ...);
    }(std::make_index_sequence<sizeof...(Ts)>{});
  }

  template <FormatSink Sink>
    requires(sizeof...(Ts) == 0)
  auto format_nth(FmtImpl<Ts...> &, size_t, FormatOptions, Sink &) {
    return 0;
  }

  template <FormatSink Sink>
  void format(Sink &sink, FormatOptions opts, FmtImpl<Ts...> &fmt) {
    (void)opts;

    const char *p = fmt.fmt;
    size_t index = 0;
    while (*p) {
      if (*p == '{') {
        p++;

        if (*p == '{') {
          sink.push('{');
          p++;
          continue;
        }

        FormatOptions options{};

        if (*p == '}') {
          format_nth(fmt, index, options, sink);
          index++;
          p++;
          continue;
        }

        if (*p == ':') {
          p++;
          if (*p == '0') {
            options.padding = '0';
            p++;
          }

          if (isdigit(*p)) {
            options.width = 0;
            while (isdigit(*p)) {
              options.width = options.width * 10 + (*p - '0');
              p++;
            }
          }

          if (tolower(*p) == 'x') {
            options.capital = (*p == 'X');
            options.base = 16;
            p++;
          }

          if (*p == 'd') {
            options.base = 10;
            p++;
          }
        }

        if (*p == '}') {
          format_nth(fmt, index, options, sink);
          index++;
          p++;
          continue;
        }
      }

      sink.push(*p++);
    }
  }
};

template <typename... Ts> auto fmt(const char *fmt, Ts &&...values) {
  return FmtImpl<Ts...>{fmt, Tuple<Ts...>{std::forward<Ts>(values)...}};
}

template <FormatSink Sink, typename... Ts>
void format(Sink &sink, const char *fmt, Ts... values) {
  FormatOptions opts{};
  auto f = ::Atlas::fmt(fmt, values...);
  format_value(f, opts, sink);
}

} // namespace Atlas
