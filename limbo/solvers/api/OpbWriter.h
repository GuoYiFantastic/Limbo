#include<fstream>
#include<string>
#include<cassert>
#include<cstdlib>
#include<sstream>
#include<iostream>


/// @brief class for writing x.opb file
/// @author Guo Yifan
class Opb
{
    public:
    std::ofstream Opbfile;
    int numVariables, numConstrain, idx;
    std::string filepath;
    
    /// @brief constructer for create and open the target x.opb file
    /// @param gdsname name of the original .gds file for creating the dir
    /// @param nV number of variables in ILP problem
    /// @param nC number of constrain in ILP problem
    /// @param i index of ILP problem, as the original .gds file can be transformed into several segments
    Opb(std::string gdsname = "test01.gds", int distance = 100, int nV = 0, int nC = 0, int i = 0):numVariables(nV), numConstrain(nC), idx(i), filepath(this->Opbpath(gdsname, distance, i))
    {
        this->Opbfile.open(filepath);
        assert(Opbfile.is_open());
        this->Opbfile << "* #variable= " << this->numVariables << " #constraint= " << this->numConstrain << std::endl << "****************************************" << std::endl;
    }
    ~Opb()
    {
        this->Opbfile.close();
    }

    /// @brief add new constrain and newline into the x.opb file
    /// @param numnz number of non-zero coefficients
    /// @param idx index vector pointed of variables
    /// @param coe 
    /// @param sense 
    /// @param rhs 
    void addconstrain(int numnz, int* idx, double* coe, char sense, double rhs)
    {
        std::string str = this->const2str(numnz, idx, coe, sense, rhs);
        this->Opbfile << str << std::endl;
    }

    void setobjmin()
    {
        this->Opbfile << "min: ";
    }
    void addobjvar(int idx, double coe)
    {
        if(coe > 0) this->Opbfile << "+" << coe*10 << " ";
        else this->Opbfile << coe*10 << " " ;
        this->Opbfile << "x" << idx << " ";
    }
    void endobj()
    {
        this->Opbfile << ";" << std::endl;
    }

    private:
    std::string Opbpath(std::string gdsname, int distance, int i)
    {
        std::string dir = "Opb/" + gdsname + "_d" + std::to_string(distance) + "/";
        if(i==0)
        {
            std::string command = "mkdir " + dir;
            system(command.c_str());
            // std::cout << "(SJTU) 【Debug】: system(command.c_str());" << std::endl;
        }
        std::string path = dir + Opbfilename(i);
        return path;
    }
    std::string Opbfilename(int idx)
    {
        std::string name;
        if(idx < 10) name = "00" + std::to_string(idx);
        else if(idx < 100) name = "0" + std::to_string(idx);
        else name = std::to_string(idx);
        return name + ".opb";
    }

    std::string const2str(int numnz, int* idx, double* coe, char sense, double rhs)
    {
        std::stringstream ss;
        for(int i=0; i<numnz; i++)
        {
            if (coe[i] > 0) ss << "+";
            ss << coe[i] << " x" << idx[i] << " ";
        }
        ss << sense << "= " << rhs << " ;";
        return ss.str();
    }
};