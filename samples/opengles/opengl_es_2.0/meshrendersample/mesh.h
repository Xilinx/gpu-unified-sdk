#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include "vec.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include "3DModel.h"


/**
 * A mesh of vertices.
 */
class Mesh
{
public:
    Mesh();
    ~Mesh();

    const Material* material_;  // Pointer to material
				
    void set_vertex_format(const std::vector<int> &format);
    void set_attrib_locations(const std::vector<int> &locations);

    void set_attrib(unsigned int pos, const LibMatrix::vec2 &v, std::vector<float> *vertex = 0);
    void set_attrib(unsigned int pos, const LibMatrix::vec3 &v, std::vector<float> *vertex = 0);
    void set_attrib(unsigned int pos, const LibMatrix::vec4 &v, std::vector<float> *vertex = 0);
    void next_vertex();
    std::vector<std::vector<float> >& vertices();

    enum VBOUpdateMethod {
        VBOUpdateMethodMap,
        VBOUpdateMethodSubData,
    };
    enum VBOUsage {
        VBOUsageStatic,
        VBOUsageStream,
        VBOUsageDynamic,
    };

    void vbo_update_method(VBOUpdateMethod method);
    void vbo_usage(VBOUsage usage);
    void interleave(bool interleave);

    void reset();
    void build_array();
    void build_vbo();
    void update_array(const std::vector<std::pair<size_t, size_t> >& ranges);
    void update_vbo(const std::vector<std::pair<size_t, size_t> >& ranges);
    void delete_array();
    void delete_vbo();

    void render_array();
    void set_material(const Material* material);
    const Material* get_material() const;  // Add a getter for material
    void render_vbo(GLuint programID);
    // New method to render a specific object
    void render_specific_object(const std::vector<Model::Vertex>& object_vertices,GLuint programID);

    typedef void (*grid_configuration_func)(Mesh &mesh, int x, int y, int n_x, int n_y,
                                            LibMatrix::vec3 &ul,
                                            LibMatrix::vec3 &ll,
                                            LibMatrix::vec3 &ur,
                                            LibMatrix::vec3 &lr);

    void make_grid(int n_x, int n_y, double width, double height,
                   double spacing, grid_configuration_func conf_func = 0);

private:
    bool check_attrib(unsigned int pos, int dim);
    std::vector<float> &ensure_vertex();
    void update_single_array(const std::vector<std::pair<size_t, size_t> >& ranges,
                             size_t n, size_t nfloats, size_t offset);
    void update_single_vbo(const std::vector<std::pair<size_t, size_t> >& ranges,
                           size_t n, size_t nfloats);

    //
    // vertex_format_ is a vector of pairs describing the attribute data.
    // pair.first is the dimension of the attribute (vec2, vec3, vec4) and
    // pair.second is the number of float values into the vertex data of that
    // attribute.  So, for example, if there are 3 attributes, a vec3 position
    // a vec3 normal and a vec2 texture coordinate, you would have a
    // vertex_format_ with 3 elements as follows: {<3, 0>, <3, 3>, <2, 6>} and
    // the total size of each vertex would be the sum of the dimensions times
    // the size of a float, or: 8 * sizeof(float) == 32 bytes
    //
    std::vector<std::pair<int, int> > vertex_format_;
    std::vector<int> attrib_locations_;
    int vertex_size_;

    std::vector<std::vector<float> > vertices_;

    std::vector<float *> vertex_arrays_;
    std::vector<GLuint> vbos_;
    std::vector<float *> attrib_data_ptr_;
    int vertex_stride_;
    bool interleave_;
    VBOUpdateMethod vbo_update_method_;
    VBOUsage vbo_usage_;
};

#endif
