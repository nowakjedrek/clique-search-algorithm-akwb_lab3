#include <algorithm>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <numeric>
#include <cmath>

using namespace std;
//struktura przechowująca ID sekwencji, sequences nukleotydową i  jakość
struct Sequence {
	string sequenceID;
	vector<char> nucleotides;
	vector<int> quality;
	vector<int> pozycja;


};

//struktura wierzchołka
struct Vertex {
	Sequence& sequence;
	size_t position;
	string nucleotides;
};


//vector<Sequence> sequences;
struct Graph {
	Graph(vector<Sequence>& sequences, size_t length) {
		for (auto& each : sequences)
			vertexGenerator(each, length);
		edgesGenerator();
	}

	vector<Vertex> vertices{};

	

	void vertexGenerator(Sequence& sequence, size_t length) {
		
			if (sequence.nucleotides.size() >= length) {
				for (size_t i = 0; i <= sequence.nucleotides.size() - length; ++i) {
					string subSequence{}; // lista podciągów
					for (size_t j = 0; j < length; ++j) {
						

						subSequence += sequence.nucleotides[i + j];
					}
					vertices.emplace_back(sequence, sequence.pozycja[i], subSequence);

				}

			}
		}
	
	vector<vector<int>> edges;
	vector<int> temp;
	void edgesGenerator() {

		for (size_t i = 0; i < vertices.size(); ++i) {
			for (size_t j = i+1; j < vertices.size(); ++j) {
				

					if (vertices[i].nucleotides == vertices[j].nucleotides) {
						if (&vertices[i].sequence != &vertices[j].sequence) {

							int diff = vertices[i].position - vertices[j].position;

							if (abs(diff) < 10 * vertices[i].nucleotides.size()) {

								temp.emplace_back(i);
								temp.emplace_back(j);
								edges.emplace_back(temp);
								temp.clear();

							
							}
						}
					}
				}
			}
		}
	int degree(int index) {
		int deg = 0;
		for (int i = 0; i < edges.size(); i++) {
			if (edges[i][0] == index || edges[i][1] == index)
				deg++;
		}
		return deg;
	}

	vector<int> connected(int index) {
		vector<int> n;
		for (int i = 0; i < edges.size(); i++) {
			if (edges[i][0] == index)
				n.push_back(edges[i][1]);
			if (edges[i][1] == index)
				n.push_back(edges[i][0]);
		}
		return n;
	}

	vector<vector<int>>  cliqueGenerator(int rozmiar) {
		//cout << "Szukanie kliki o rozmiarze: " << rozmiar << endl;
		if (rozmiar < 2) {
			cout << "Nie znaleziono żadnej kliki!" << endl;
			return{};
		}

		vector<vector<int>> cliques;
		vector<int> probable_clique, temp1, temp2;

		for (int i = 0; i < edges.size(); ++i) {
			for (int x = 0; x < 2; ++x) {
				//cout << "deg: " << degree(edges[i][x]) <<endl;
				if (degree(edges[i][x]) >= rozmiar - 1) {

					probable_clique.push_back(edges[i][x]);
					temp1 = connected(edges[i][x]);
					probable_clique.insert(probable_clique.end(), temp1.begin(), temp1.end());
					temp1.clear();
					sort(probable_clique.begin(), probable_clique.end());

					for (int j = 0; j < probable_clique.size(); j++) {
						temp1.push_back(probable_clique[j]);
						temp2 = connected(probable_clique[j]);
						temp1.insert(temp1.end(), temp2.begin(), temp2.end());
						temp2.clear();
						sort(temp1.begin(), temp1.end());
						
						for (int k = 0; k < probable_clique.size(); k++) {
							if (find(temp1.begin(), temp1.end(), probable_clique[k]) == temp1.end()) {
								//cout << "usuwanie z potential clique"<<endl;
								probable_clique.erase(find(probable_clique.begin(), probable_clique.end(), probable_clique[k]));
								--k;
							}
						}
						temp1.clear();
					}
					if (probable_clique.size() > rozmiar - 1) {
						
						cliques.push_back(probable_clique);
					
					}
					probable_clique.clear();
				}
			}
		}
		if (cliques.empty()) {
			
			return cliqueGenerator(rozmiar - 1);
		}
		else {
			
			return cliques;
		}

	}
	
	
};



int main() {

	string filename;
	int length;
	int threshold;

	cout << "Podaj nazwe pliku: ";
	cin >> filename;



	fstream inputFasta("path_to_fastafile" + filename + ".fasta");
	fstream inputQual("path_to_qualityfile" + filename + ".qual");
	if (!inputFasta || !inputQual) {
		cout << "Nieprawidłowa nazwa pliku!!!";
	}
	else {
		vector<Sequence> sequences;
		string lineFasta;
		string lineQual;
		while (getline(inputFasta, lineFasta, '>') && getline(inputQual, lineQual, '>')) {
			if (lineFasta.empty() || lineQual.empty())
				continue;
			string sequenceID = lineFasta.substr(0, lineFasta.find(' '));

			lineFasta.erase(0, lineFasta.find('\n'));
			lineFasta.erase(remove(lineFasta.begin(), lineFasta.end(), '\n'), lineFasta.end());//usuwanie spacji pomiędzy wartościami
			vector<char> nucleotides{ lineFasta.begin(), lineFasta.end() };
			lineQual.erase(0, lineQual.find('\n'));
			istringstream numberinteger(lineQual);
			vector<int> quality = { istream_iterator<int>(numberinteger), istream_iterator<int>() };
			vector<int> pozycja;
			
			int pos=0;
			for (size_t i = 0; i < nucleotides.size(); i++) {
				pos++;
				pozycja.push_back(pos);
			}

			sequences.emplace_back(sequenceID, nucleotides, quality, pozycja);
			
		}

		cout << "Podaj dlugosc podciagu: (miedzy 4 a 9) ";
		cin >> length;

		while (length < 4 || length > 9)
		{
			cout << "Nieprawidlowa dlugosc podciagu!!" << "\n";
			cout << "Podaj dlugosc podciagu: (miedzy 4 a 9) ";
			cin >> length;
		}

		cout << "Podaj prog wiarygodnosci: ";
		cin >> threshold;
		while (threshold < 0) {
			cout << "Prog wiarygodnosci musi byc wiekszy od 0" << endl;
			cout << "Podaj prog wiarygodnosci: ";
			cin >> threshold;
		}
		
		for (auto& sequence : sequences) {

			for (int i = 0, j = sequence.quality.size(); i < j; ++i) {
				if (sequence.quality[i] < threshold) {
					sequence.nucleotides.erase(sequence.nucleotides.begin() + i);
					sequence.pozycja.erase(sequence.pozycja.begin() + i);
					sequence.quality.erase(sequence.quality.begin() + i);
					--i;
					--j;

				}

			}
			
		}
		
		
		Graph graph(sequences, length);
		
		vector<vector<int>> klika = graph.cliqueGenerator(sequences.size());
		int index = klika[0][0];
		//cout << klika[0][0]<<endl;
		if (!klika.empty()) {
			for (int j = 0; j < klika[0].size(); j++) {
				index = klika[0][j];
				cout << "ID:" << graph.vertices[index].sequence.sequenceID
					<< " Motyw:" << graph.vertices[index].nucleotides
					<< " Pozycja:" << graph.vertices[index].position<<endl;
			}
		}
			
		else
		{
			cout << "Nie znaleziono kliki";
		}

		
	}
	return 0;
}