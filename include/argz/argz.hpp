// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace argz
{
   template <class T>
   using ref = std::reference_wrapper<T>;

   template <class T>
   using ref_opt = ref<std::optional<T>>;

   using var = std::variant<
      ref<bool>,
      ref<int32_t>,
      ref<uint32_t>,
      ref<int64_t>,
      ref<uint64_t>,
      ref<std::string>,
      ref_opt<int32_t>,
      ref_opt<uint32_t>,
      ref_opt<int64_t>,
      ref_opt<uint64_t>,
      ref_opt<std::string>>;
   
   struct ids_t final {
      std::string_view id{};
      char alias = '\0';
   };

   struct arg_t final {
      ids_t ids{};
      var value;
      std::string_view help{};
   };

   using options = std::vector<arg_t>;

   struct about final {
      std::string_view description{}, version{};
      bool print_help_when_no_options = true;
      bool help{};
   };

   namespace detail
   {
      template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
      template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
      
      inline std::string_view parse_var(const char* c) {
         auto start = c;
         while (*c != '\0') {
            ++c;
         }
         return { start, static_cast<size_t>(c - start) };
      }
      
      inline void parse(const char* const c, var& v)
      {
         if (c) {
            const auto str = parse_var(c);
            std::visit(overloaded{
               [&](ref<std::string>& x) { x.get() = str; },
               [&](ref<bool>& x) { x.get() = str == "true" ? true : false; },
               [&]<typename T>(ref_opt<T>& x_opt) {
                  auto temp = T{};
                  auto temp_var = var{ ref<T>{ temp } };
                  parse(c, temp_var);
                  x_opt.get().emplace(std::get<ref<T>>(temp_var).get());
               },
               [&](auto& x) { x.get() = static_cast<typename std::decay_t<decltype(x)>::type>(std::stol(std::string(str))); }
               }, v);
         }
      }

      inline std::string to_string(const var& v) {
         return std::visit(overloaded {
            [](const ref<std::string>& x) { return x.get(); },
            [](const ref_opt<std::string>& x) {
               const auto has_value = x.get().has_value();
               if (has_value) return x.get().value();
               else return std::string{ };
            },
            []<typename T>(const ref_opt<T>& x_opt) {
               const auto has_value = x_opt.get().has_value();
               if (has_value) return std::to_string(x_opt.get().value());
               else return std::string{ };
            },
            [](const auto& x) { return std::to_string(x.get()); },
         }, v);
      }
   }
   
   inline void help(const about& about, const options& opts)
   {
      std::cout << about.description << '\n';
      std::cout << "Version: " << about.version << '\n';
      
      std::cout << '\n' << R"(-h, --help       write help to console)" << '\n';
      std::cout << R"(-v, --version    write the version to console)" << '\n';

      for (auto& [ids, v, h] : opts)
      {
         if (ids.alias != '\0') {
            std::cout << '-' << ids.alias << ", --" << ids.id;
         }
         else {
            std::cout << (ids.id.size() == 1 ? "-" : "--") << ids.id;
         }
         std::cout << "    " << h << ", default: " << detail::to_string(v) << '\n';
      }
      std::cout << '\n';
   }

   template <class int_t, class char_ptr_t, std::enable_if_t<std::is_pointer_v<char_ptr_t>, int> = 0>
   inline void parse(about& about, options& opts, const int_t argc, char_ptr_t argv)
   {
      if (argc == 1) {
         if (about.print_help_when_no_options) {
            help(about, opts);
         }
         return;
      }
      
      auto get_id = [&](char alias) -> std::string_view {
         for (auto& x : opts) {
            if (x.ids.alias == alias) {
               return x.ids.id;
            }
         }
         return {};
      };

      for (int_t i = 1; i < argc; ++i) {
         const char* flag = argv[i];
         if (*flag != '-') {
            throw std::runtime_error("Expected '-'");
         }
         ++flag;
         
         std::string_view str;
         if (*flag == '-') {
            ++flag;
         }
         str = detail::parse_var(flag);
         if (str == "h" || str == "help") {
            help(about, opts);
            continue;
         }
         if (str == "v" || str == "version") {
            std::cout << "Version: " << about.version << '\n';
            continue;
         }
         if (str.size() == 1) {
            str = get_id(*flag);
            if (str.empty()) {
               throw std::runtime_error("Invalid alias flag '-' for: " + std::string(str));
            }
         }
         if (str.empty()) { break; }
         
         for (auto& [ids, v, h] : opts) {
            if (ids.id == str) {
               if (std::holds_alternative<ref<bool>>(v)) {
                  std::get<ref<bool>>(v).get() = true;
               }
               else {
                  detail::parse(argv[++i], v);
               }
            }
         }
      }
   }
}
