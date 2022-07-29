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
               throw std::runtime_error("unepected '-'");
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
      else if (*c == ' ' || *c == '"') {
         while (*c == ' ' || *c == '"') {
            ++c;
         }
         auto start = c;
         while (*c != '\0' && *c != ' ' && *c != '"') {
            ++c;
         }
         if (c != start) {

            ret.emplace_back(std::string(std::string_view{ start, static_cast<size_t>(c - start) }));
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
   bool boolean = true;

   argz::options opts{
      { { "input", 'i' }, input, "the input file"},
      { { "study", 's' }, study, "a study file"},
      { { "number" }, number, "input an int"},
      { { "boolean" }, boolean, "a boolean" },
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
      parse_string(R"(program.exe -i some_file --study study_file --boolean --number 12345)");

      expect(eq(input, std::string("some_file")));
      expect(eq(study, std::string("study_file")));
      expect(eq(number, 12345));
      expect(eq(boolean, true));
   };

   test("test1") = [&] {
      parse_string(R"(program.exe -i some/path --study s --boolean --number 12)");

      expect(eq(input, std::string("some/path")));
      expect(eq(study, std::string("s")));
      expect(eq(number, 12));
      expect(eq(boolean, true));
   };

   test("test2") = [&] {
       parse_string(R"(program.exe -i)");
   };

   test("test3") = [&] {
       parse_string(R"(program.exe -i "some/path" --study "s" --boolean --number 22 )");
       expect(eq(input, std::string("some/path")));
       expect(eq(study, std::string("s")));
       expect(eq(number, 22));
       expect(eq(boolean, true));
   };

   test("test4") = [&] {
       parse_string(R"(program.exe -i                                 some/path --study s --boolean -- )");
       expect(eq(input, std::string("some/path")));
       expect(eq(study, std::string("s")));
       expect(eq(number, 22));
       expect(eq(boolean, true)); 
   };

   test("test5") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe -h)"); }));
   };

   test("test6") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe -i some/path --study s --boolean   --        )"); }));
       expect(eq(input, std::string("some/path")));
       expect(eq(study, std::string("s")));
       expect(eq(boolean, true));

   };

   test("test7") = [&] {
       expect(throws([&] {parse_string(R"(program.exe -i some/path --study s --boolean  27 --number true)"); }));
   };
   
   test("test8") = [&] {
       expect(nothrow([&] {parse_string(R"(program.exe - )"); }));
   };
   
   test("test-dashes") = [&] {
       parse_string(R"(program.exe -i ./some-path-with-dashes.txt )");
       expect(eq(input, std::string("./some-path-with-dashes.txt"))) << "actual: " << input;
   };

   return 0;
}
