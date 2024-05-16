/**
 * @file   GurobiApi.h
 * @brief  Gurobi API wrapper using its C API. 
 * @author Yibo Lin
 * @date   Nov 2014
 */

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp> 
#include <limbo/solvers/Solvers.h>
#include <limbo/solvers/api/OpbWriter.h>
#include </home/fantastic_na/OpenMPL/OpenMPL/src/globals.h>
#include <limbo/solvers/api/minisatp_uti.h>

/// namespace for Limbo
namespace limbo 
{ 
/// namespace for Solvers 
namespace solvers 
{


/// @brief NeuroSat API with @ref limbo::solvers::LinearModel
/// @tparam T coefficient type 
/// @tparam V variable type 
template <typename T, typename V>
class NeuroSatApi 
{
    public:
        /// @brief linear model type for the problem 
        typedef LinearModel<T, V> model_type; 
        /// @nowarn
        typedef typename model_type::coefficient_value_type coefficient_value_type; 
        typedef typename model_type::variable_value_type variable_value_type; 
        typedef typename model_type::variable_type variable_type;
        typedef typename model_type::constraint_type constraint_type; 
        typedef typename model_type::expression_type expression_type; 
        typedef typename model_type::term_type term_type; 
        typedef typename model_type::property_type property_type; 
        /// @endnowarn
        
        /// @brief constructor 
        /// @param model pointer to the model of problem 
        NeuroSatApi(model_type* model)
            : m_model(model)
        {
        }
        /// @brief destructor 
        ~NeuroSatApi()
        {
        }

        /// @brief 将ILP问题写入opb文件，遵循minisatp所需格式。
        /// @return opb文件所在路径。
        std::string write_opb_file()
        {
            // std::cout << "(SJTU)【Debug】:write_opb_file()" << std::endl;
            //opb file writer
            Opb opbwriter(GDSNAME, distance, m_model->numVariables(),m_model->constraints().size(),IDX);
            IDX ++;
            // create objective 
            opbwriter.setobjmin();
            for (typename std::vector<term_type>::const_iterator it = m_model->objective().terms().begin(), ite = m_model->objective().terms().end(); it != ite; ++it)
            {
                opbwriter.addobjvar(it->variable().id(), it->coefficient());
            }
            opbwriter.endobj();

            // create constraints 
            std::vector<int> vIdx; 
            std::vector<double> vValue; 
            for (unsigned int i = 0, ie = m_model->constraints().size(); i < ie; ++i)
            {
                constraint_type const& constr = m_model->constraints().at(i);

                vIdx.clear(); 
                vValue.clear();
                for (typename std::vector<term_type>::const_iterator it = constr.expression().terms().begin(), ite = constr.expression().terms().end(); it != ite; ++it)
                {
                    vIdx.push_back(it->variable().id()); 
                    vValue.push_back(it->coefficient());
                }
                opbwriter.addconstrain(constr.expression().terms().size(), &vIdx[0], &vValue[0], constr.sense(), constr.rightHandSide());
            }
            // std::cout << "(SJTU) Successfully write opbfile: " <<  "..." << std::endl;
            return opbwriter.filepath;
        }

        // std::vector<int> neurosat()
        // {
        //     int len = m_model->numVariables();
        //     std::vector<int> raw_solution(len);
        //     for(int i = 0; i < len; ++i) 
        //     {
        //         std::cin >> raw_solution[i];
        //     }
        //     return raw_solution;
        // }

        

        // std::vector<int> minisatp(std::string opbpath)
        // {
             
        // }

        int setSolution(std::vector<int> raw_solution)
        {
            for (unsigned int i = 0; i < m_model->numVariables(); ++i)
            {
                variable_type var = m_model->variable(i); 
                int value = raw_solution[var.id()];
                m_model->setVariableSolution(var, value);
            }
            return 0;
        }
        
        /// @brief API to run the algorithm 
        /// @param 
        SolverProperty operator()()
        {
            std::string opbpath = write_opb_file();
            std::vector<int> raw_solution = minisatp(opbpath);
            int error = setSolution(raw_solution);
            return OPTIMAL;
        }


    protected:
        /// @brief copy constructor, forbidden 
        /// @param rhs right hand side 
        NeuroSatApi(NeuroSatApi const& rhs);
        /// @brief assignment, forbidden 
        /// @param rhs right hand side 
        NeuroSatApi& operator=(NeuroSatApi const& rhs);

        model_type* m_model; ///< model for the problem 
};

} // namespace solvers
} // namespace limbo
