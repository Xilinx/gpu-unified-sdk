#ifndef MODEL3D_H
#define MODEL3D_H

#define GLES_VERSION 2 

#include <GLES2/gl2.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include "vec.h"
#include <memory>
#include <filesystem>

// Forward declare the mesh object. We don't need the whole header here.
class Mesh;

enum ModelFormat
{
    MODEL_INVALID,
    MODEL_3DS,
    MODEL_OBJ
};

struct Material {
      std::string name;
      LibMatrix::vec3 Ka;  // Ambient color
      LibMatrix::vec3 Kd;  // Diffuse color
      LibMatrix::vec3 Ks;  // Specular color
      float Ns; // Shininess
      float d;  // Transparency (1.0 = opaque, 0.0 = fully transparent)
    };


/**
 * A descriptor for a model file.
 */
class ModelDescriptor
{
    std::string name_;
    std::filesystem::path pathname_;
    ModelFormat format_;
    ModelDescriptor();
public:
    ModelDescriptor(const std::string& name, ModelFormat format,
                    const std::filesystem::path& pathname) :
        name_(name),
        pathname_(pathname),
        format_(format) {}
    ~ModelDescriptor() {}
    const std::filesystem::path& pathname() const { return pathname_; }
    ModelFormat format() const { return format_; }
};

typedef std::map<std::string, std::unique_ptr<ModelDescriptor>> ModelMap;

/**
 * A model as loaded from a 3D object data file.
 */
class Model
{
public:
    typedef enum {
        AttribTypePosition = 1,
        AttribTypeNormal,
        AttribTypeTexcoord,
        AttribTypeTangent,
        AttribTypeBitangent,
        AttribTypeCustom
    } AttribType;

    struct Face {
        LibMatrix::uvec3 v;
        LibMatrix::uvec3 n;
        LibMatrix::uvec3 t;
        unsigned int which; // mask of which values are relevant for this face.
        static const unsigned int OBJ_FACE_V;
        static const unsigned int OBJ_FACE_T;
        static const unsigned int OBJ_FACE_N;
    };


    struct Vertex {
        LibMatrix::vec3 v;
        LibMatrix::vec3 n;
        LibMatrix::vec2 t;
        LibMatrix::vec3 nt;
        LibMatrix::vec3 nb;
    };

    struct Object {
        Object(const std::string &name) : name(name) {}
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<Face> faces;
    };

    Model() : gotTexcoords_(false), gotNormals_(false) {}
    ~Model() {}

    bool load(const std::string& name);

    // Method to get material by name
    const Material* getMaterialByName(const std::string& name) const;

    bool needTexcoords() const { return !gotTexcoords_; }
    bool needNormals() const { return !gotNormals_; }
    void calculate_texcoords();
    void calculate_normals();
    void convert_to_mesh(Mesh &mesh);
    void convert_to_mesh(Mesh &mesh,
                         const std::vector<std::pair<AttribType, int>> &attribs);
    const LibMatrix::vec3& minVec() const { return minVec_; }
    const LibMatrix::vec3& maxVec() const { return maxVec_; }
    static const ModelMap& find_models();

    // Provide access to the objects in the model
    const std::vector<Object>& getObjects() const { return objects_; }

private:
    bool gotTexcoords_;
    bool gotNormals_;
     std::map<std::string, Material> materials_;

    void append_object_to_mesh(const Object &object, Mesh &mesh,
                               int p_pos, int n_pos, int t_pos,
                               int nt_pos, int nb_pos);
    bool load_3ds(const std::filesystem::path &filename);
    bool load_obj(const std::filesystem::path &filename);
    bool load_mtl(const std::filesystem::path& filename);
    void obj_get_attrib(const std::string& description, LibMatrix::vec2& v);
    void obj_get_attrib(const std::string& description, LibMatrix::vec3& v);
    void obj_face_get_index(const std::string& tuple, unsigned int& which,
        unsigned int& v, unsigned int& t, unsigned int& n);
    void obj_get_face(const std::string& description, Face& face);
    void obj_get_faces(const std::string& description, std::vector<Face>& outFaces);

    // For vertices of the bounding box for this model.
    void compute_bounding_box(const Object& object);
    LibMatrix::vec3 minVec_;
    LibMatrix::vec3 maxVec_;
    std::vector<Object> objects_;
};

#endif
