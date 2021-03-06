// CenariosByRegionGrowing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Declarando a nuvem de pontos do paciente.
pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud <pcl::PointXYZRGB>);



/**
 * Faz a leitura da nuvem de pontos do paciente.
 */
void ler_paciente_nuvem_pontos_rgb()
{
	std::cout << "Iniciando Leitura da Nuvem de Pontos do Paciente... ";
	if (pcl::io::loadPCDFile <pcl::PointXYZRGB>("paciente.pcd", *cloud) == -1)
	{
		std::cout << "Cloud reading failed." << std::endl;

	}
	std::cout << "OK!" << std::endl << std::endl;

	std::cout << "___________________________________________________________" << std::endl << std::endl;
}


void ler_paciente_nuvem_pontos_lab()
{
	std::cout << "Iniciando Leitura da Nuvem de Pontos do Paciente... ";
	if (pcl::io::loadPLYFile <pcl::PointXYZRGB>("paciente_lab_opencv_aprox_fake_rgb_pos_cc.ply", *cloud) == -1)
	{
		std::cout << "Cloud reading failed." << std::endl;

	}
	std::cout << "OK!" << std::endl << std::endl;

	std::cout << "___________________________________________________________" << std::endl << std::endl;
}


/**
 * Salva os parametros usados na segmentação num arquivo txt.
 */
void save_parameters_in_txt(int scenario, float distanceThreshold, float pointColorThreshold, float regionColorThreshold, float minClusterSize)
{
	std::cout << "Gravando aquivo com os parametros de segmentacao usados... ";


	std::stringstream nome_arquivo;
	nome_arquivo << "Cenario_" << scenario << "/Parametros_Segmentacao_Cenario_" << scenario << ".txt";



	std::stringstream ss;
	ss << "Parametros para Segmentacao do Cenario " << scenario << std::endl << std::endl;


	ss << "distanceThreshold: " << distanceThreshold << std::endl;
	ss << "pointColorThreshold: " << pointColorThreshold << std::endl;
	ss << "regionColorThreshold: " << regionColorThreshold << std::endl;
	ss << "minClusterSize: " << minClusterSize << std::endl;

	FILE * pFile;
	pFile = fopen(nome_arquivo.str().c_str(), "w");
	if (pFile != NULL)
	{
		fputs(ss.str().c_str(), pFile);
		fclose(pFile);
	}

	std::cout << "OK! " << std::endl << std::endl;


	std::cout << ss.str() << std::endl << std::endl;
}

/**
* Realiza a segmentação pelo regon growing.
*/
void region_growing(int scenario, float distanceThreshold, float pointColorThreshold, float regionColorThreshold, float minClusterSize) {


	pcl::search::Search <pcl::PointXYZRGB>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZRGB> >(new pcl::search::KdTree<pcl::PointXYZRGB>);

	

	std::cout << "Definindo Parametros de Segmentacao... ";
	
	pcl::RegionGrowingRGB<pcl::PointXYZRGB> reg;
	reg.setInputCloud(cloud);
	//reg.setIndices(indices);	
	reg.setSearchMethod(tree);
	reg.setDistanceThreshold(distanceThreshold);
	reg.setPointColorThreshold(pointColorThreshold);
	reg.setRegionColorThreshold(regionColorThreshold);
	reg.setMinClusterSize(minClusterSize); 
	
	std::cout << "OK!" << std::endl << std::endl;


	// Salvando os parametros num arquivo de texto.
	save_parameters_in_txt(scenario, distanceThreshold, pointColorThreshold, regionColorThreshold, minClusterSize);






	std::cout << "Extraindo os clusters (vai demorar)... ";
	
	std::vector <pcl::PointIndices> clusters;
	reg.extract(clusters);
	std::cout << "OK!" << std::endl << std::endl;
	
	std::cout << "    - Gerou " << clusters.size() << " Clusters." << std::endl;
	std::cout << "    - Segmentacao Finzalida." << std::endl << std::endl;




	std::cout << "Tentando gravar nuvem segmentada no PLY...";
	pcl::PointCloud <pcl::PointXYZRGB>::Ptr colored_cloud = reg.getColoredCloud();

	std::stringstream nome_arquivo_segmentado;
	nome_arquivo_segmentado << "Cenario_" << scenario << "/paciente_segmentado_clusters_" << std::to_string(clusters.size());
	nome_arquivo_segmentado << ".ply";

	pcl::io::savePLYFile(nome_arquivo_segmentado.str(), *colored_cloud);
	std::cout << "OK!" << std::endl << std::endl;





	//std::cout << "Deseja Gerar os Arquivos de cada Cluster separadamente? S/N" << std::endl;
	std::string opcao;
	//getline(std::cin, opcao);

	opcao = "S";

	if (opcao == "S") {
		std::cout << "Iniciando Geracaoo de Arquivos Separados de Cada Cluster... ";

		pcl::PointCloud<pcl::PointXYZRGB> cloud_w;

		//pcl::PCDWriter writer;

		pcl::PLYWriter writer;

		for (int ii = 0; ii < clusters.size(); ii++)
		{
			cloud_w.clear();
			for (size_t i = 0; i < clusters[ii].indices.size(); ++i)
			{
				cloud_w.push_back(cloud->points[clusters[ii].indices[i]]);
			}

			std::stringstream ss;
			ss << "Cenario_" << scenario << "/pct_seg_cluster_" << ii << ".ply";
			writer.write<pcl::PointXYZRGB>(ss.str(), cloud_w, false);
			//std::cerr << "Saved " << cloud_w.points.size() << " data points to file	" << ii << std::endl;
		}


		std::cout << "OK!" << std::endl << std::endl;

	}


	std::cout << "Execucao Finalizada." << std::endl;
}



void make_scenario(int scenario, float distanceThreshold, float pointColorThreshold, float regionColorThreshold, float minClusterSize)
{
	std::cout << "Iniciando Processo de Segmentacao do Cenario " << scenario << "... " << std::endl << std::endl;

	std::cout << "Criando o Diretorio do Cenario " << scenario << "... ";

	std::stringstream cenario;
	cenario << "Cenario_" << scenario;

	
	boost::filesystem::path dir(cenario.str());
	if (boost::filesystem::create_directory(dir))
	{
		std::cerr << "OK! Diretorio criado com sucesso: " << cenario.str() << std::endl << std::endl;
	} else
		std::cerr << "OK!" << std::endl << std::endl;





	region_growing(scenario, distanceThreshold, pointColorThreshold, regionColorThreshold, minClusterSize);






	std::cout << "___________________________________________________________" << std::endl << std::endl;
}



void scenarios_rgb()
{

	// Lendo a nuvem de pontos do paciente.
	ler_paciente_nuvem_pontos_rgb();

	/**
	* Variando o PointColorThreshold
	*/
	make_scenario(22, 3, 1, 3.5, 600);     // Não segmenta, muito pesado, dá estoruro de memoria. Razão: x86
	make_scenario(23, 3, 1.5, 3.5, 600);   // Não segmenta, muito pesado, dá estoruro de memoria. Razão: x86
	/*make_scenario(24, 3, 2, 3.5, 600);
	make_scenario(25, 3, 2.5, 3.5, 600);
	make_scenario(26, 3, 3, 3.5, 600);
	make_scenario(27, 3, 3.5, 3.5, 600);
	make_scenario(28, 3, 4, 3.5, 600);
	make_scenario(29, 3, 4.5, 3.5, 600);
	make_scenario(30, 3, 5, 3.5, 600);
	make_scenario(31, 3, 5.5, 3.5, 600);
	make_scenario(32, 3, 6, 3.5, 600);*/

	/**
	 * Variando o RegionColorThreshold
	 */
	/*make_scenario(33, 3, 3.5, 1, 600);
	make_scenario(34, 3, 3.5, 1.5, 600);
	make_scenario(35, 3, 3.5, 2, 600);
	make_scenario(36, 3, 3.5, 2.5, 600);
	make_scenario(37, 3, 3.5, 3, 600);
	make_scenario(38, 3, 3.5, 3.5, 600);
	make_scenario(39, 3, 3.5, 4, 600);
	make_scenario(40, 3, 3.5, 4.5, 600);
	make_scenario(41, 3, 3.5, 5, 600);
	make_scenario(42, 3, 3.5, 5.5, 600);
	make_scenario(43, 3, 3.5, 6, 600);*/

	/**
	 * Variando o MinClusterSize.
	*/
	/*make_scenario(10, 3, 3.5, 1, 1000);
	make_scenario(11, 3, 3.5, 1, 1500);
	make_scenario(12, 3, 3.5, 1, 2000);
	make_scenario(13, 3, 3.5, 1, 2500);
	make_scenario(14, 3, 3.5, 1, 3000);
	make_scenario(15, 3, 3.5, 1, 3500);
	make_scenario(16, 3, 3.5, 1, 4000);
	make_scenario(17, 3, 3.5, 1, 4500);
	make_scenario(18, 3, 3.5, 1, 5000);
	make_scenario(19, 3, 3.5, 1, 5500);
	make_scenario(20, 3, 3.5, 1, 6000);
	make_scenario(21, 3, 3.5, 1, 600); */
}

void scenarios_lab()
{
	// Lendo a nuvem de pontos do paciente.
	ler_paciente_nuvem_pontos_lab();

	/**
	 * Experimentos Preliminares
	 **/
	/*make_scenario(1, 1, 1, 2, 100);
	make_scenario(2, 2, 1.5, 2.5, 200);
	make_scenario(3, 3, 2, 3.5, 300);
	make_scenario(4, 4, 2.5, 4, 400);
	make_scenario(5, 3, 3, 3, 500);
	make_scenario(6, 4, 4, 4.5, 600);
	make_scenario(7, 3, 2.5, 3, 700);
	make_scenario(8, 4, 3, 4.5, 800);
	make_scenario(9, 3, 3.5, 3.5, 900);*/
	
	/*make_scenario(10, 3, 2, 3.5, 600);
	make_scenario(11, 3, 2, 5, 600);
	make_scenario(12, 3, 2, 3.5, 1000);
	make_scenario(13, 3, 2, 5, 1000);*/

	/*make_scenario(14, 3, 2, 5, 1500);
	make_scenario(15, 3, 2, 5.5, 1800);
	make_scenario(16, 3, 2, 5, 2000);*/



	/**
	* Variando a Vizinhança
	*/
	/*make_scenario(47, 1, 2.5, 6, 600);
	make_scenario(48, 1.5, 2.5, 6, 600);
	make_scenario(49, 2, 2.5, 6, 600);
	make_scenario(50, 2.5, 2.5, 6, 600);
	make_scenario(51, 3.5, 2.5, 6, 600);
	make_scenario(52, 4, 2.5, 6, 600);*/

	/**
	* Variando o MinClusterSize. Feito em 11/06/2018.
	*/
	/*make_scenario(10, 3, 2, 5, 600);
	make_scenario(11, 3, 2, 5, 1000);
	make_scenario(12, 3, 2, 5, 1500);
	make_scenario(13, 3, 2, 5, 2000);
	make_scenario(14, 3, 2, 5, 2500);
	make_scenario(15, 3, 2, 5, 3000);
	make_scenario(16, 3, 2, 5, 3500);
	make_scenario(17, 3, 2, 5, 4000);
	make_scenario(18, 3, 2, 5, 4500);
	make_scenario(19, 3, 2, 5, 5000);
	make_scenario(20, 3, 2, 5, 5500);
	make_scenario(21, 3, 2, 5, 6000);*/


	/**
	* Variando o PointColorThreshold. Feito em 11/06/2018
	*/
	/*make_scenario(22, 3, 2.0, 5, 1000);     
	make_scenario(23, 3, 2.1, 5, 1000);
	make_scenario(24, 3, 2.2, 5, 1000);
	make_scenario(25, 3, 2.3, 5, 1000);
	make_scenario(26, 3, 2.4, 5, 1000);
	make_scenario(27, 3, 2.5, 5, 1000);
	make_scenario(28, 3, 2.6, 5, 1000);
	make_scenario(29, 3, 2.7, 5, 1000);
	make_scenario(30, 3, 2.8, 5, 1000);
	make_scenario(31, 3, 2.9, 5, 1000);
	make_scenario(32, 3, 3.0, 5, 1000);*/

	/**
	 * Variando o RegionColorThreshold
	 */
	/*make_scenario(33, 3, 2, 3.0, 1000);
	make_scenario(34, 3, 2, 3.5, 1000);
	make_scenario(35, 3, 2, 4, 1000);
	make_scenario(36, 3, 2, 4.5, 1000);
	make_scenario(37, 3, 2, 5, 1000);
	make_scenario(38, 3, 2, 5.5, 1000);
	make_scenario(39, 3, 2, 6, 1000);
	make_scenario(40, 3, 2, 6.5, 1000);
	make_scenario(41, 3, 2, 7, 1000);
	make_scenario(42, 3, 2, 7.5, 1000);
	make_scenario(43, 3, 2, 8, 1000);*/

	

	/**
	* Segmentando de acordo com os melhores parametros obtidos.
	*/
	make_scenario(44, 3, 2.0, 3.5, 2000);
	make_scenario(45, 3, 2.1, 5.0, 2500);
	make_scenario(46, 3, 2.2, 5.5, 1500);
}


int main()
{
	std::cout << "PROGRAMA DE SEGMENTACAO POR REGION GROWING COLOR BASED INICIADO! " << std::endl << std::endl;

	
	scenarios_lab();

	




























	//             scenario, distanceThreshold, pointColorThreshold, regionColorThreshold, minClusterSize
	//make_scenario(    1,           3.5,                4,                  3,                    1000      );
	//make_scenario(	  2,			3,				   3,				   3,					 1900      );
	//make_scenario(	  3,		   3.5,				   4,				   3,					 600       );

	//make_scenario(    4,		   3.5,				   4,				   3,				     5000	   );


	//make_scenario(12, 3, 2, 2, 6500);


	/*make_scenario(1, 3, 2, 1, 1000);
	make_scenario(2, 3, 2, 1, 1500);
	make_scenario(3, 3, 2, 1, 1800);
	make_scenario(4, 3, 2, 1, 2000);
	make_scenario(5, 3, 2, 1, 2200);
	make_scenario(6, 3, 2, 1, 2500);
	make_scenario(7, 3, 2, 1, 3000);
	make_scenario(8, 3, 2, 1, 4000);
	make_scenario(9, 3, 2, 1, 5000);
	make_scenario(10, 3, 2, 1, 5500);
	make_scenario(11, 3, 2, 1, 6000);
	make_scenario(13, 3, 2, 4, 6500);

	make_scenario(14, 1, 1.5, 4, 600);*/







	



	/**
	 * Segmentando de acordo com os melhores parametros obtidos.
	 */
	/*make_scenario(44, 3, 2.5, 6, 600);
	make_scenario(45, 3, 3, 5.5, 1000);
	make_scenario(46, 3, 3.5, 5, 1500);*/













	//             scenario, distanceThreshold, pointColorThreshold, regionColorThreshold, minClusterSize

	


	
	
	
	//make_scenario(2);
	//make_scenario(3);
	//make_scenario(4);


	getchar();
    return 0;
}

