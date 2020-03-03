#define _CRT_SECURE_NO_WARNINGS

#include "ObjParser.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>

bool ParseObj(const std::string& file, HMesh& mesh)
{
	mesh.Cleanup();

	if (!Validate(file))
		return false;

	std::ifstream ifs(file, std::ifstream::in);
	std::string line;
	std::vector<ObjFace> objFaces;

	while (ifs.good())
	{
		getline(ifs, line);
		ParseLine(objFaces, mesh, line);
	}

	for (ObjFace objFace : objFaces)
	{
		AddFace(mesh, objFace.vids);
	}

	ConnectTwins(mesh);
	SortEdges(mesh);
}

bool Validate(const std::string& file)
{
	std::string ext = file.substr(file.find_last_of('.') + 1);

	if (ext != "obj")
		return false;

	std::ifstream ifs(file, std::ifstream::in);
	if (!ifs.good())
		return false;

	return true;
}

void ParseLine(std::vector<ObjFace>& objFaces, HMesh& mesh, const std::string& line)
{
	if (line[0] == 'v' && line[1] == ' ')
	{
		HVertex* v = new HVertex();
		sscanf(line.c_str(), "v %f %f %f", &v->position.x, &v->position.y, &v->position.z);
		v->id = mesh.vertices.size() + 1;
		mesh.vertices.push_back(v);
		v = nullptr;
	}
	else if (line[0] == 'f' && line[1] == ' ')
	{
		std::stringstream tokenizer(line);
		std::vector<std::string> tokens;
		std::string token;

		while (getline(tokenizer, token, ' '))
			tokens.push_back(token);

		ObjFace f;
		f.id = objFaces.size() + 1;
		int vid;
		for (int i = 1; i < tokens.size(); i++)
		{
			sscanf(tokens[i].c_str(), "%d", &vid);
			f.vids.push_back(vid);
		}
		objFaces.push_back(f);
	}
	else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
	{
		glm::vec3 v(0.0f);
		sscanf(line.c_str(), "vt %f %f", &v.x, &v.y);
		mesh.vt.push_back(v);
	}
}

void AddFace(HMesh& mesh, const std::vector<int>& vids)
{
	HFace* f = new HFace();
	f->id = mesh.faces.size() + 1;

	HEdge* e = new HEdge();
	e->tail = mesh.vertices[vids[0] - 1];
	mesh.vertices[vids[0] - 1]->edge = e;
	e->face = f;
	e->id = mesh.edges.size() + 1;
	f->edge = e;
	mesh.edges.push_back(e);
	HEdge* prev = e;
	for (int i = 1; i < vids.size(); ++i)
	{
		HEdge* e = new HEdge();
		e->tail = mesh.vertices[vids[i] - 1];
		mesh.vertices[vids[i] - 1]->edge = e;
		e->face = f;						
		e->id = mesh.edges.size() + 1;
		prev->next = e;

		prev = e;
		mesh.edges.push_back(e);
		e = nullptr;
	}
	mesh.edges.back()->next = f->edge;

	f->CalculateNormal();
	mesh.faces.push_back(f);
	prev = nullptr;
	e = nullptr;
	f = nullptr;
}

void ConnectTwins(HMesh& mesh)
{
	std::map<std::pair<int, int>, HEdge*> twins;

	std::pair<int, int> vPair;
	int vid1, vid2;
	HEdge* curr;
	for (HEdge* e : mesh.edges)
	{
		vid1 = e->tail->id;
		vid2 = e->next->tail->id;

		if (vid1 < vid2)
			vPair = std::pair<int, int>(vid2, vid1);
		else
			vPair = std::pair<int, int>(vid1, vid2);

		std::map<std::pair<int, int>, HEdge*>::iterator twinIter = twins.find(vPair);

		if (twinIter == twins.end())
		{
			twins.insert(std::pair<std::pair<int, int>, HEdge*>(vPair, e));
		}
		else
		{
			HEdge* eTwin = twinIter->second;
			e->twin = eTwin;
			eTwin->twin = e;
			e->duplicate = false;
			eTwin->duplicate = true;
		}
	}

	for (auto e : mesh.edges)
		e->prev = e->Prev();
}

void SortEdges(HMesh& mesh)
{
	std::vector<HEdge*> l1, l2;

	for (HEdge* e : mesh.edges)
	{
		if (!e->duplicate)
			l1.push_back(e);
		else
			l2.push_back(e);
	}

	mesh.edges.clear();

	for (HEdge* e : l1)
	{
		mesh.edges.push_back(e);
	}

	for (HEdge* e : l2)
	{
		mesh.edges.push_back(e);
	}
}