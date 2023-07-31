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

#include "argz/argz.hpp"

inline std::vector<std::string> string_to_vector(const std::string& str)
{
   std::vector<std::string> ret;

   const char* c = str.data();
   while (*c != '\0')
   {
      if (*c == '-') {
         auto start = c;
         ++c;
         if (*c == '-') {
            ++c;
            if (*c == '-') {
               throw std::runtime_error("Unepected -");
            }
            else if (*c==' ') {
                break;
            }
           
            while (*c != '\0' && *c != '-' && *c != ' ') {
               ++c;
            }

            ret.emplace_back(std::string(std::string_view{ start, static_cast<size_t>(c - start) }));
         }
         else {
            while (*c != '\0' && *c != '-' && *c != ' ') {
               ++c;
            }

            ret.emplace_back(std::string(std::string_view{ start, static_cast<size_t>(c - start) }));
         }
      }
      else if (*c == '"') {
         ++c;
         auto start = c;
         while (*c != '\0' && *c != '"') {
            ++c;
         }
         if (*c == '\0') {
            throw std::runtime_error("Expected \"");
         }
         
         ret.emplace_back(std::string(std::string_view{ start, static_cast<size_t>(c - start) }));
         ++c; // skip last quote
      }
      else if (*c == ' ') {
         while (*c == ' ') {
            ++c;
         }
      }
      else {
         auto start = c;
         while (*c != ' ' && *c != '\0') {
            ++c;
         }

         ret.emplace_back(std::string(std::string_view{ start, static_cast<size_t>(c - start) }));
      }
   }
   
   return ret;
}

#define BOOST_UT_DISABLE_MODULE
#include "boost/ut.hpp"

using namespace boost::ut;
using namespace boost::ut::literals;

int main(int argc, char* argv[])
{
   constexpr std::string_view version = "1.2.3";
   argz::about about{ "My program description", version };

   std::string input{};
   std::string study{};
   int number = 123;
   bool boolean = false;

   std::optional<std::string> input_opt{};
   std::optional<std::string> study_opt{};
   std::optional<int> number_opt{};

   argz::options opts{
      { { "input", 'i' }, input, "the input file"},
      { { "study", 's' }, study, "a study file"},
      { { "number" }, number, "input an int"},
      { { "boolean" }, boolean, "a boolean" },
      { { "input_opt" }, input_opt, "the input file"},
      { { "study_opt" }, study_opt, "a study file"},
      { { "number_opt" }, number_opt, "input an int"},
   };

   try {
      argz::parse(about, opts, argc, argv);
   }
   catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
   }
   
   std::vector<const char*> buffers;

   auto to_buffers = [&](const auto& strings)
   {
      buffers.resize(strings.size());
      for (size_t i = 0; i < buffers.size(); ++i) {
         buffers[i] = strings[i].data();
      }
   };

   std::vector<std::string> strings;

   auto parse_string = [&](const auto& str)
   {
      strings = string_to_vector(str);
      to_buffers(strings);
      
      argz::parse(about, opts, buffers.size(), buffers.data());
   };

   test("test0") = [&] {
      boolean = false; // Reset not to be right on default
      parse_string(R"(program.exe -i some_file --study study_file --boolean --number 12345)");

      expect(input == "some_file");
      expect(study == "study_file");
      expect(number == 12345);
      expect(boolean == true);


   };

   test("test1") = [&] {
      boolean = false; // Reset not to be right on default
      parse_string(R"(program.exe -i some/path --study s --boolean --number 12)");

      expect(input == "some/path");
      expect(study == "s");
      expect(number == 12);
      expect(boolean == true);
   };

   test("test2") = [&] {
       parse_string(R"(program.exe -i)");
   };

   test("test3") = [&] {
       boolean = false; // Reset not to be right on default
       parse_string(R"(program.exe -i "some/path" --study "s" --boolean --number 22 )");
       expect(input == "some/path");
       expect(study == "s");
       expect(number == 22);
       expect(boolean == true);
   };

   test("test4") = [&] {
       boolean = false; // Reset not to be right on default
       parse_string(R"(program.exe -i                                 some/path --study s --boolean -- )");
       expect(input == "some/path");
       expect(study == "s");
       expect(number == 22);
       expect(boolean == true);
   };

   test("test5") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe -h)"); }));
   };

   test("test6") = [&] {
       boolean = false; // Reset not to be right on default
       expect(nothrow([&] {parse_string(R"(program.exe -i some/path --study s --boolean   --        )"); }));
       expect(input == "some/path");
       expect(study == "s");
       expect(boolean == true);

   };

   test("test7") = [&] {
       boolean = false; // Reset not to be right on default
       expect(throws([&] {parse_string(R"(program.exe -i some/path --study s --boolean  27 --number true)"); }));
   };
   
   test("test8") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe - )"); }));
   };
   
   test("test-dashes") = [&] {
       parse_string(R"(program.exe -i ./some-path-with-dashes.txt )");
       expect(input == std::string("./some-path-with-dashes.txt")) << "actual: " << input;
   };
   
   test("quoted_path") = [&] {
       parse_string(R"(program.exe -i "./../some quoted path.txt" )");
       expect(input == "./../some quoted path.txt") << "actual: " << input;
   };

   test("no_options_is_possible") = [&] {
      parse_string(R"(program.exe)");
      
      expect(nothrow([&] {argz::parse_when_no_options_is_possible(about, opts, buffers.size(), buffers.data()); }));
   };
      
   test("opt_test0") = [&] {
      boolean = false; // Reset not to be right on default
      input_opt = {};
      study_opt = {};
      number_opt = {};

      parse_string(R"(program.exe --input_opt some_file --study_opt study_file --boolean --number_opt 12345)");

      expect(input_opt.value() == "some_file");
      expect(study_opt.value() == "study_file");
      expect(number_opt.value() == 12345);
      expect(boolean == true);


   };

   test("opt_test1") = [&] {
      boolean = false; // Reset not to be right on default
      input_opt = {};
      study_opt = {};
      number_opt = {};

      parse_string(R"(program.exe --input_opt some/path --study_opt s --boolean --number_opt 12)");

      expect(input_opt.value() == "some/path");
      expect(study_opt.value() == "s");
      expect(number_opt.value() == 12);
      expect(boolean == true);
   };

   test("opt_test2") = [&] {
       parse_string(R"(program.exe --input_opt)");
   };

   test("opt_test3") = [&] {
       boolean = false; // Reset not to be right on default
       input_opt = {};
       study_opt = {};
       number_opt = {};

       parse_string(R"(program.exe --input_opt "some/path" --study_opt "s" --boolean --number_opt 22 )");
       expect(input_opt.value() == "some/path");
       expect(study_opt.value() == "s");
       expect(number_opt.value() == 22);
       expect(boolean == true);
   };

   test("opt_test4") = [&] {
       boolean = false; // Reset not to be right on default
       input_opt = {};
       study_opt = {};
       number_opt = {};

       parse_string(R"(program.exe --input_opt                                 some/path --study_opt s --boolean -- )");
       expect(input_opt.value() == "some/path");
       expect(study_opt == "s");
       expect(!number_opt.has_value());
       expect(boolean == true);
   };

   test("opt_test5") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe -h)"); }));
   };

   test("opt_test6") = [&] {
       boolean = false; // Reset not to be right on default
       input_opt = {};
       study_opt = {};
       number_opt = {};

       expect(nothrow([&] {parse_string(R"(program.exe --input_opt some/path --study_opt s --boolean   --        )"); }));

       expect(input_opt.value() == "some/path");
       expect(study_opt.value() == "s");
       expect(!number_opt.has_value());
       expect(boolean == true);

   };

   test("opt_test7") = [&] {
       expect(throws([&] {parse_string(R"(program.exe --input_opt some/path --study_opt s --boolean  27 --number_opt true)"); }));
   };

   test("opt_test8") = [&] {
       boolean = false; // Reset not to be right on default
       input_opt = {};
       study_opt = {};
       number_opt = {};

       expect(nothrow([&] {parse_string(R"(program.exe - )"); }));

       expect(!input_opt.has_value());
       expect(!study_opt.has_value());
       expect(!number_opt.has_value());
       expect(boolean == false);
   };

   test("opt_test-dashes") = [&] {
       parse_string(R"(program.exe --input_opt ./some-path-with-dashes.txt )");
       expect(input_opt.value() == std::string("./some-path-with-dashes.txt")) << "actual: " << input_opt.value_or("Empty");
   };

   test("opt_quoted_path") = [&] {
       parse_string(R"(program.exe --input_opt "./../some quoted path.txt" )");
       expect(input_opt.value() == "./../some quoted path.txt") << "actual: " << input_opt.value_or("Empty");
   };

   return 0;
}
