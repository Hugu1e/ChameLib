#ifndef CHAMELIB_COMMON_TEST_H
#define CHAMELIB_COMMON_TEST_H

#include <gtest/gtest.h>
#include <stack>
#include <map>
#include <chrono>
#include <cmath>
#include "utils/Logger.h"
#include "curve/params.h"

const int diff_max_len = 17;

/*
random in G1, random in G2, random in GT, random in Zr,
hash to G1, hash to G2, hash to GT, hash to Zr,
mul in G1, mul in G2, mul in GT, mul in Zr,
pow in G1, pow in G2, pow in GT, pow in Zr,
pairing
*/
const double op_time[][diff_max_len] = {
    {2.604492, 2.619141, 0.613281, 0.069336, 5.110352, 5.688477, 0.316406, 0.000977, 0.011719, 0.011719, 0.000977, 0.000000, 2.480469, 2.429688, 0.261719, 0.010742, 2.087891},
    {2.036133, 1.958984, 0.256836, 0.074219, 1.714844, 1.755859, 0.111328, 0.003906, 0.028320, 0.029297, 0.003906, 0.000977, 34.380859, 37.666992, 4.953125, 1.054688, 56.241211},
    {5.753906, 6.287109, 1.008789, 0.051758, 28.879883, 28.822266, 0.954102, 0.000977, 0.025391, 0.027344, 0.000977, 0.000000, 5.794922, 5.791992, 0.190430, 0.013672, 7.063477},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {2.350586, 4.184570, 44.275391, 0.070312, 0.082031, 0.169922, 41.157227, 0.000977, 0.004883, 0.009766, 0.044922, 0.000000, 2.054688, 3.788086, 14.175781, 0.034180, 64.085938},
    {1.033203, 8.191406, 1.371094, 0.069336, 0.045898, 25.054688, 0.838867, 0.000977, 0.002930, 0.033203, 0.009766, 0.000000, 0.875977, 7.642578, 1.834961, 0.011719, 5.815430},
    {1.467773, 10.943359, 1.882812, 0.074219, 0.211914, 34.840820, 1.329102, 0.000977, 0.003906, 0.040039, 0.009766, 0.000000, 1.256836, 9.928711, 2.411133, 0.011719, 7.953125},
    {1.819336, 13.482422, 1.890625, 0.069336, 0.132812, 38.913086, 1.333984, 0.000977, 0.003906, 0.042969, 0.011719, 0.000000, 1.594727, 12.483398, 2.960938, 0.024414, 9.842773},
    {1.473633, 10.751953, 1.791016, 0.070312, 0.252930, 36.133789, 1.250977, 0.000977, 0.003906, 0.040039, 0.010742, 0.000000, 1.267578, 10.061523, 2.512695, 0.011719, 8.015625},
    {1.103516, 8.748047, 1.443359, 0.068359, 0.112305, 28.408203, 0.943359, 0.000977, 0.002930, 0.034180, 0.008789, 0.000000, 0.924805, 7.944336, 1.914062, 0.011719, 6.271484},
    {1.357422, 10.296875, 1.463867, 0.067383, 0.224609, 31.337891, 1.058594, 0.000977, 0.003906, 0.038086, 0.008789, 0.000000, 1.227539, 9.487305, 2.164062, 0.011719, 7.420898},
    {1.020508, 2.031250, 26.307617, 0.073242, 0.023438, 0.049805, 24.512695, 0.000977, 0.003906, 0.006836, 0.043945, 0.000000, 0.875977, 1.834961, 8.389648, 0.010742, 35.358398},
    {1.000977, 14.871094, 7.923828, 0.069336, 0.036133, 99.000977, 6.630859, 0.000977, 0.002930, 0.072266, 0.032227, 0.000000, 0.794922, 14.565430, 4.838867, 0.010742, 17.639648}
};

template <typename T>
class BaseTest : public testing::TestWithParam<T>  {
    protected:
        int repeat = 100;

        bool out_file = true;
        bool check_visiable = true;
        bool visiable = true;

        std::stack<std::string> current_test_name;
        std::stack<std::chrono::_V2::system_clock::time_point> ts;
        std::map<std::string, double> average_time;

        FILE *out = NULL;

        void SetUp() override {
            std::string filename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
            size_t pos = filename.find('/');
            if (pos != std::string::npos) {
                filename = filename.substr(pos+1, filename.size() - pos -1);
                filename += ".txt";
            }
            out = fopen(filename.c_str(), "a");
            fflush(out);
        }

        void TearDown() override {
            fprintf(out, "\n\n");
            fclose(out);
        }

        void OutTime(std::string name, std::string id, double us) {
            us /= 1000;
            // if(out_file) fprintf(out, "%s %s time: %lf ms.\n", name.c_str(), id.c_str(), us);
            // if(visiable) printf("%s %s time: %lf ms.\n", name.c_str(), id.c_str(), us);

            if(average_time.find(name) == average_time.end()){
                average_time[name] = us;
            }else{
                average_time[name] += us;
            }
        }

        void average(){
            for(auto &p : average_time){
                p.second /= repeat;
                if(out_file) fprintf(out, "%s average time: %lf ms.\n", p.first.c_str(), p.second);
                if(visiable) printf("%s average time: %lf ms.\n", p.first.c_str(), p.second);
            }
        }

        void average(std::string name, int repeat_time){
            average_time[name] /= repeat_time;
        }

        bool check_time(int type, int ops[diff_max_len], std::string test_name) {
            if(average_time.find(test_name) == average_time.end()){
                throw std::runtime_error("check_time(): wrong test name");
            }
            double real_time = average_time[test_name];

            std::string curve_name = Curve::curve_names[type];

            double expect_time = 0;
            for(int i = 0; i < diff_max_len; i++) expect_time += op_time[type][i] * ops[i];
            double diff = real_time - expect_time;
            double diff_percent = (diff) * 100 / (expect_time + 1e-6);
            if(expect_time < 1e-6) diff_percent = 0;

            if(out_file)fprintf(out, "curve %s %s: expect_time = %lf ms, real_time = %lf ms, diff = ", curve_name.c_str(), test_name.c_str(),  expect_time, real_time);
            if(check_visiable)printf("curve %s %s: expect_time = %lf ms, real_time = %lf ms, diff = ", curve_name.c_str(), test_name.c_str(), expect_time, real_time);
            
            if(fabs(diff) < 0.05){
                if(out_file)fprintf(out,"%.2lf", diff);
                if(check_visiable)printf("%.2lf", diff);
            } 
            else if(fabs(diff) < 0.1){
                if(out_file) fprintf(out, "%.2lf", diff);
                if(check_visiable)printf("\033[1;33;40m%.2lf\033[0m", diff);
            } 
            else{
                if(out_file) fprintf(out, "%.2lf", diff);
                if(check_visiable)printf("\033[1;31;40m%.2lf\033[0m", diff);
            }
            if(out_file) fprintf(out, " ms (");
            if(check_visiable)printf(" ms (");


            if(fabs(diff_percent) < 5){
                if(out_file) fprintf(out, "%.2lf%%", diff_percent);
                if(check_visiable)printf("%.2lf%%", diff_percent);
            } 
            else if(fabs(diff_percent) < 10){
                if(out_file) fprintf(out, "%.2lf%%", diff_percent);
                if(check_visiable)printf("\033[1;33;40m%.2lf%%\033[0m", diff_percent);
            } 
            else {
                if(out_file) fprintf(out, "%.2lf%%", diff_percent);
                if(check_visiable)printf("\033[1;31;40m%.2lf%%\033[0m", diff_percent);
            }

            if(out_file) fprintf(out, ")\n");
            if(check_visiable)printf(")\n");

            return fabs(diff_percent) < 10 || fabs(diff) < 0.1;
        }
        
        void start(std::string current_test_name) {
            if(visiable)std::cout<<"——————————" << current_test_name <<" start——————————" << std::endl;
            this->current_test_name.push(current_test_name);
            ts.push(std::chrono::system_clock::now());
        }

        void start(std::string current_test_name, bool visiable) {
            if(visiable)std::cout<<"——————————" << current_test_name <<" start——————————" << std::endl;
            this->current_test_name.push(current_test_name);
            ts.push(std::chrono::system_clock::now());
        }

        void end(std::string current_test_name) {
            std::chrono::_V2::system_clock::time_point te = std::chrono::system_clock::now();
            if(this->current_test_name.empty() || this->current_test_name.top() != current_test_name) throw std::runtime_error("end(): wrong test pair");
            OutTime(current_test_name, ::testing::UnitTest::GetInstance()->current_test_info()->name(), std::chrono::duration_cast<std::chrono::microseconds>(te - ts.top()).count());
            if(visiable)std::cout<<"——————————" << current_test_name <<" end——————————" << std::endl;
            this->current_test_name.pop();
            ts.pop();
        }

        void end(std::string current_test_name, bool visiable) {
            std::chrono::_V2::system_clock::time_point te = std::chrono::system_clock::now();
            if(this->current_test_name.empty() || this->current_test_name.top() != current_test_name) throw std::runtime_error("end(): wrong test pair");
            OutTime(current_test_name, ::testing::UnitTest::GetInstance()->current_test_info()->name(), std::chrono::duration_cast<std::chrono::microseconds>(te - ts.top()).count());
            if(visiable)std::cout<<"——————————" << current_test_name <<" end——————————" << std::endl;
            this->current_test_name.pop();
            ts.pop();
        }

        void UpdateProcBar(int current, int total){
            if(visiable) return;

            char bar[101];
            memset(bar,'\0',sizeof(bar));
            
            char lable[] = "|/-\\";
            int len = strlen(lable);
            double rate = (current*100.0)/total;
            int cnt = 0;
            int loop_count = (int)rate;
            while(cnt <= loop_count)
            {
                bar[cnt++] = '#';
            }
            printf("[%-100s][%.1lf%%][%c]\r",bar,rate,lable[cnt%len]);
            if(current == total) printf("\n");
            fflush(stdout);
        }
};

#endif  // CHAMELIB_COMMON_TEST_H