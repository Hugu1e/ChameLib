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
    {2.917969, 2.966797, 0.500977, 0.070312, 5.791992, 5.848633, 0.345703, 0.000977, 0.011719, 0.011719, 0.000977, 0.000000, 2.630859, 2.639648, 0.281250, 0.010742, 2.217773},
    {2.037109, 2.096680, 0.275391, 0.074219, 1.824219, 1.824219, 0.114258, 0.003906, 0.029297, 0.026367, 0.004883, 0.000977, 38.249023, 37.925781, 4.788086, 1.226562, 57.399414},
    {6.480469, 6.376953, 1.102539, 0.082031, 27.805664, 28.082031, 0.944336, 0.000977, 0.026367, 0.025391, 0.000977, 0.000000, 5.743164, 5.750000, 0.188477, 0.010742, 7.562500},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {2.271484, 4.073242, 44.955078, 0.071289, 0.077148, 0.166992, 41.684570, 0.000977, 0.004883, 0.005859, 0.043945, 0.000000, 2.059570, 3.728516, 14.328125, 0.031250, 63.917969},
    {1.023438, 8.074219, 1.350586, 0.070312, 0.042969, 24.103516, 0.842773, 0.000977, 0.004883, 0.036133, 0.009766, 0.000000, 0.871094, 7.589844, 1.828125, 0.009766, 5.939453},
    {1.384766, 10.798828, 1.873047, 0.066406, 0.204102, 34.539062, 1.302734, 0.000977, 0.004883, 0.040039, 0.010742, 0.000000, 1.234375, 9.872070, 2.376953, 0.012695, 7.708008},
    {1.710938, 13.176758, 1.860352, 0.071289, 0.131836, 39.364258, 1.337891, 0.000977, 0.004883, 0.041992, 0.010742, 0.000000, 1.600586, 12.269531, 2.979492, 0.025391, 9.636719},
    {1.429688, 11.559570, 1.799805, 0.072266, 0.252930, 33.970703, 1.255859, 0.000977, 0.003906, 0.041992, 0.010742, 0.000000, 1.270508, 10.044922, 2.405273, 0.011719, 8.127930},
    {1.054688, 7.901367, 1.308594, 0.073242, 0.107422, 24.378906, 0.891602, 0.000977, 0.002930, 0.032227, 0.007812, 0.000000, 0.897461, 7.704102, 1.847656, 0.010742, 6.160156},
    {1.317383, 10.286133, 1.646484, 0.082031, 0.270508, 27.428711, 1.009766, 0.000000, 0.002930, 0.022461, 0.004883, 0.000000, 1.215820, 9.362305, 2.202148, 0.012695, 7.417969},
    {0.993164, 1.939453, 26.019531, 0.073242, 0.020508, 0.047852, 24.277344, 0.000977, 0.002930, 0.005859, 0.043945, 0.000000, 0.852539, 1.811523, 8.274414, 0.010742, 35.073242},
    {0.948242, 15.711914, 8.475586, 0.081055, 0.035156, 110.878906, 6.709961, 0.000977, 0.002930, 0.075195, 0.027344, 0.000000, 0.809570, 14.789062, 5.080078, 0.011719, 18.474609}
};

template <typename T>
class BaseTest : public testing::TestWithParam<T>  {
    protected:
        int repeat = 100;

        bool out_file = true;
        bool check_visiable = true;
        bool visiable = false;

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

        void end(std::string current_test_name) {
            std::chrono::_V2::system_clock::time_point te = std::chrono::system_clock::now();
            if(this->current_test_name.empty() || this->current_test_name.top() != current_test_name) throw std::runtime_error("end(): wrong test pair");
            OutTime(current_test_name, ::testing::UnitTest::GetInstance()->current_test_info()->name(), std::chrono::duration_cast<std::chrono::microseconds>(te - ts.top()).count());
            if(visiable)std::cout<<"——————————" << current_test_name <<" end——————————" << std::endl;
            this->current_test_name.pop();
            ts.pop();
        }
};

#endif  // CHAMELIB_COMMON_TEST_H