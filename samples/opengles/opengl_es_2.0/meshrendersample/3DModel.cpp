#include "mesh.h"
#include "3DModel.h"
#include "vec.h"
#include "math.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <memory>
#include <cfloat>
#include <climits>
#include "util.h"
#include "mat.h"

using std::string;
using std::vector;
using LibMatrix::vec2;
using LibMatrix::vec3;
using LibMatrix::uvec3;

static inline void read_from_file(std::istream& file, void *dst, size_t nmemb, size_t size) {
    file.read(reinterpret_cast<char *>(dst), nmemb * size);
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    char *p = reinterpret_cast<char *>(dst);
    for (size_t i = 0; i < nmemb; ++i) {
        if (size == 2) {
            std::swap(p[size*i], p[size*i+1]);
        } else if (size == 4) {
            std::swap(p[size*i], p[size*i+3]);
            std::swap(p[size*i+1], p[size*i+2]);
        }
    }
#endif
}

#define read_or_fail(file, dst, nmemb, size) do { \
    read_from_file((file), (dst), (nmemb), (size)); \
    if (file.gcount() < (std::streamsize)((nmemb)*(size))) { \
        return false; \
    } \
} while(0);

/**
 * Computes the bounding box for a Model::Object.
 *
 * @param object the Model object
 */
void
Model::compute_bounding_box(const Object& object)
{
    float minX(FLT_MAX);
    float maxX(FLT_MIN);
    float minY(FLT_MAX);
    float maxY(FLT_MIN);
    float minZ(FLT_MAX);
    float maxZ(FLT_MIN);
    for (vector<Vertex>::const_iterator vIt = object.vertices.begin(); vIt != object.vertices.end(); vIt++)
    {
        const vec3& curVtx = vIt->v;
        if (curVtx.x() < minX)
        {
            minX = curVtx.x();
        }
        if (curVtx.x() > maxX)
        {
            maxX = curVtx.x();
        }
        if (curVtx.y() < minY)
        {
            minY = curVtx.y();
        }
        if (curVtx.y() > maxY)
        {
            maxY = curVtx.y();
        }
        if (curVtx.z() < minZ)
        {
            minZ = curVtx.z();
        }
        if (curVtx.z() > maxZ)
        {
            maxZ = curVtx.z();
        }
    }
    maxVec_ = vec3(maxX, maxY, maxZ);
    minVec_ = vec3(minX, minY, minZ);
}

/**
 * Appends the vertices of a Model::Object to a Mesh.
 *
 * @param object the object to append
 * @param mesh the mesh to append to
 * @param p_pos the attribute position to use for the 'position' attribute
 * @param n_pos the attribute position to use for the 'normal' attribute
 * @param t_pos the attribute position to use for the 'texcoord' attribute
 */
void Model::append_object_to_mesh(const Object &object, Mesh &mesh,
                                  int p_pos, int n_pos, int t_pos,
                                  int nt_pos, int nb_pos)
{
    //printf("Appending object with %zu vertices and %zu faces\n", object.vertices.size(), object.faces.size());

    size_t faceIndex = 0; // To track which face we're processing

    for (vector<Face>::const_iterator faceIt = object.faces.begin();
         faceIt != object.faces.end();
         faceIt++, faceIndex++)
    {

        const Face &face = *faceIt;

        // Check if the face indices are within bounds
        if (face.v.x() >= object.vertices.size() || face.v.y() >= object.vertices.size() || face.v.z() >= object.vertices.size()) {
            printf("Error: Face vertex index out of bounds at face %zu\n", faceIndex);
            continue;
        }

        const Vertex &v1 = object.vertices[face.v.x()];
        const Vertex &v2 = object.vertices[face.v.y()];
        const Vertex &v3 = object.vertices[face.v.z()];

        // Handle separate texture coordinates if they exist
        bool separate_t(face.which & Face::OBJ_FACE_T);
        const Vertex &t1 = object.vertices[separate_t ? face.t.x() : face.v.x()];
        const Vertex &t2 = object.vertices[separate_t ? face.t.y() : face.v.y()];
        const Vertex &t3 = object.vertices[separate_t ? face.t.z() : face.v.z()];

        // Handle separate normals if they exist
        bool separate_n(face.which & Face::OBJ_FACE_N);

        Vertex n1, n2, n3;

        if (separate_n) {
            if (face.n.x() >= object.vertices.size() || face.n.y() >= object.vertices.size() || face.n.z() >= object.vertices.size()) {
                // Use default normals if out of bounds
                n1.n = vec3(0.2242, 0.7413, 0.6327);
                n2.n = vec3(0.3629, 0.6852, 0.6315);
                n3.n = vec3(0.3387, 0.6976, 0.6315);
            } else {
                // Use the normal indices if they're valid
                n1 = object.vertices[face.n.x()];
                n2 = object.vertices[face.n.y()];
                n3 = object.vertices[face.n.z()];
            }
        } else {
            // If no separate normals, use vertex positions or fallback normals
            n1 = object.vertices[face.v.x()];
            n2 = object.vertices[face.v.y()];
            n3 = object.vertices[face.v.z()];
        }

        // Add the vertices to the mesh, checking for valid attribute positions
        mesh.next_vertex();
        if (p_pos >= 0) mesh.set_attrib(p_pos, v1.v);
        if (n_pos >= 0) mesh.set_attrib(n_pos, n1.n);
        if (t_pos >= 0) mesh.set_attrib(t_pos, t1.t);
        if (nt_pos >= 0) mesh.set_attrib(nt_pos, v1.nt);
        if (nb_pos >= 0) mesh.set_attrib(nb_pos, v1.nb);

        mesh.next_vertex();
        if (p_pos >= 0) mesh.set_attrib(p_pos, v2.v);
        if (n_pos >= 0) mesh.set_attrib(n_pos, n2.n);
        if (t_pos >= 0) mesh.set_attrib(t_pos, t2.t);
        if (nt_pos >= 0) mesh.set_attrib(nt_pos, v2.nt);
        if (nb_pos >= 0) mesh.set_attrib(nb_pos, v2.nb);

        mesh.next_vertex();
        if (p_pos >= 0) mesh.set_attrib(p_pos, v3.v);
        if (n_pos >= 0) mesh.set_attrib(n_pos, n3.n);
        if (t_pos >= 0) mesh.set_attrib(t_pos, t3.t);
        if (nt_pos >= 0) mesh.set_attrib(nt_pos, v3.nt);
        if (nb_pos >= 0) mesh.set_attrib(nb_pos, v3.nb);
    }

}

/**
 * Converts a model to a mesh using the default attributes bindings.
 *
 * The default attributes and their order is: Position, Normal, Texcoord
 *
 * @param mesh the mesh to populate
 */
void
Model::convert_to_mesh(Mesh &mesh)
{
    std::vector<std::pair<AttribType, int> > attribs;

    attribs.push_back(std::pair<AttribType, int>(AttribTypePosition, 3));
    attribs.push_back(std::pair<AttribType, int>(AttribTypeNormal, 3));
    attribs.push_back(std::pair<AttribType, int>(AttribTypeTexcoord, 2));

    convert_to_mesh(mesh, attribs);
}

/**
 * Converts a model to a mesh using custom attribute bindings.
 *
 * The attribute bindings are pairs of <AttribType, dimensionality>.
 *
 * @param mesh the mesh to populate
 * @param attribs the attribute bindings to use
 */
void
Model::convert_to_mesh(Mesh &mesh,
                       const std::vector<std::pair<AttribType, int> > &attribs)
{


    std::vector<int> format;
    int p_pos = -1;
    int n_pos = -1;
    int t_pos = -1;
    int nt_pos = -1;
    int nb_pos = -1;

    mesh.reset();


    for (std::vector<std::pair<AttribType, int> >::const_iterator ai = attribs.begin();
         ai != attribs.end();
         ai++)
    {
        format.push_back(ai->second);
        if (ai->first == AttribTypePosition)
            p_pos = ai - attribs.begin();
        else if (ai->first == AttribTypeNormal)
            n_pos = ai - attribs.begin();
        else if (ai->first == AttribTypeTexcoord)
            t_pos = ai - attribs.begin();
        else if (ai->first == AttribTypeTangent)
            nt_pos = ai - attribs.begin();
        else if (ai->first == AttribTypeBitangent)
            nb_pos = ai - attribs.begin();
    }

    mesh.set_vertex_format(format);


    for (std::vector<Object>::const_iterator iter = objects_.begin();
         iter != objects_.end();
         iter++)
    {


        append_object_to_mesh(*iter, mesh, p_pos, n_pos, t_pos, nt_pos, nb_pos);
    }

}

void
Model::calculate_texcoords()
{
    if (gotTexcoords_)
        return;

    // Since the model didn't come with texcoords, and we don't actually know
    // if it came with normals, either, we'll use positional spherical mapping
    // to generate texcoords for the model.  See:
    // http://www.mvps.org/directx/articles/spheremap.htm for more details.
    vec3 centerVec = maxVec_ + minVec_;
    centerVec *= 0.5;
    
    for (std::vector<Object>::iterator iter = objects_.begin();
         iter != objects_.end();
         iter++)
    {
        Object &object = *iter;
        for (vector<Vertex>::iterator vertexIt = object.vertices.begin();
             vertexIt != object.vertices.end();
             vertexIt++)
        {
            Vertex& curVertex = *vertexIt;
            vec3 vnorm(curVertex.v - centerVec);
            vnorm.normalize();
            curVertex.t.x(asinf(vnorm.x()) / M_PI + 0.5);
            curVertex.t.y(asinf(vnorm.y()) / M_PI + 0.5);
        }
    }

    gotTexcoords_ = true;
}

/**
 * Calculates the normal vectors of the model vertices.
 */
void
Model::calculate_normals()
{
    if (gotNormals_)
        return;

    LibMatrix::vec3 n;

    for (std::vector<Object>::iterator iter = objects_.begin();
         iter != objects_.end();
         iter++)
    {
        Object &object = *iter;

        for (vector<Face>::const_iterator f_iter = object.faces.begin();
             f_iter != object.faces.end();
             f_iter++)
        {
            const Face &face = *f_iter;
            Vertex &a = object.vertices[face.v.x()];
            Vertex &b = object.vertices[face.v.y()];
            Vertex &c = object.vertices[face.v.z()];

            /* Calculate normal */
            n = LibMatrix::vec3::cross(b.v - a.v, c.v - a.v);
            n.normalize();
            a.n += n;
            b.n += n;
            c.n += n;

            LibMatrix::vec3 q1(b.v - a.v);
            LibMatrix::vec3 q2(c.v - a.v);
            LibMatrix::vec2 u1(b.t - a.t);
            LibMatrix::vec2 u2(c.t - a.t);
            float det = (u1.x() * u2.y() - u2.x() * u1.y());

            /* Calculate tangent */
            LibMatrix::vec3 nt;
            nt.x(det * (u2.y() * q1.x() - u1.y() * q2.x()));
            nt.y(det * (u2.y() * q1.y() - u1.y() * q2.y()));
            nt.z(det * (u2.y() * q1.z() - u1.y() * q2.z()));
            nt.normalize();
            a.nt += nt;
            b.nt += nt;
            c.nt += nt;

            /* Calculate bitangent */
            LibMatrix::vec3 nb;
            nb.x(det * (u1.x() * q2.x() - u2.x() * q1.x()));
            nb.y(det * (u1.x() * q2.y() - u2.x() * q1.y()));
            nb.z(det * (u1.x() * q2.z() - u2.x() * q1.z()));
            nb.normalize();
            a.nb += nb;
            b.nb += nb;
            c.nb += nb;
        }

        for (vector<Vertex>::iterator v_iter = object.vertices.begin();
             v_iter != object.vertices.end();
             v_iter++)
        {
            Vertex &v = *v_iter;
            /* Orthogonalize */
            v.nt = (v.nt - v.n * LibMatrix::vec3::dot(v.nt, v.n));
            v.n.normalize();
            v.nt.normalize();
            v.nb.normalize();
        }
    }

    gotNormals_ = true;
}

/**
 * Load a model from a 3DS file.
 *
 * @param filename the name of the file
 *
 * @return whether loading succeeded
 */
bool
Model::load_3ds(const std::filesystem::path &filename)
{
    printf("Attempting to load 3DS model from file: %s\n", filename.c_str()); // Add this line


    Object *object(0);


    const std::unique_ptr<std::istream> input_file_ptr(Util::get_resource(filename));
    std::istream& input_file(*input_file_ptr);

    if (!input_file) {
        return false;
    }

    // Loop to scan the whole file
    while (!input_file.eof()) {
        uint16_t chunk_id;
        uint32_t chunk_length;

        // Read the chunk header
        read_from_file(input_file, &chunk_id, 1, 2);
        if (input_file.gcount() == 0) {
            continue;
        }
        else if (input_file.gcount() < 2) {
            return false;
        }

        //Read the length of the chunk
        read_or_fail(input_file, &chunk_length, 1, 4);

	// Print chunk information
         printf("Chunk ID: 0x%04x, Length: %u\n", chunk_id, chunk_length);

        switch (chunk_id)
        {
            //----------------- MAIN3DS -----------------
            // Description: Main chunk, contains all the other chunks
            // Chunk ID: 4d4d
            // Chunk Length: 0 + sub chunks
            //-------------------------------------------
            case 0x4d4d:
                break;

            //----------------- EDIT3DS -----------------
            // Description: 3D Editor chunk, objects layout info
            // Chunk ID: 3d3d (hex)
            // Chunk Length: 0 + sub chunks
            //-------------------------------------------
            case 0x3d3d:
                break;

            //--------------- EDIT_OBJECT ---------------
            // Description: Object block, info for each object
            // Chunk ID: 4000 (hex)
            // Chunk Length: len(object name) + sub chunks
            //-------------------------------------------
            case 0x4000:
                {
                std::stringstream ss;
                unsigned char c = 1;

                for (int i = 0; i < 20 && c != '\0'; i++) {
                    read_or_fail(input_file, &c, 1, 1);
                    ss << c;
                }

		std::string objectName = ss.str();  // Capture the object name
                printf("Detected object with name: %s\n", objectName.c_str());
                objects_.push_back(Object(ss.str()));
                object = &objects_.back();
		// Debugging statement to verify object creation
                printf("Created new object: %s\n", object->name.c_str());
                }
                break;

            //--------------- OBJ_TRIMESH ---------------
            // Description: Triangular mesh, contains chunks for 3d mesh info
            // Chunk ID: 4100 (hex)
            // Chunk Length: 0 + sub chunks
            //-------------------------------------------
            case 0x4100:
                break;

            //--------------- TRI_VERTEXL ---------------
            // Description: Vertices list
            // Chunk ID: 4110 (hex)
            // Chunk Length: 1 x unsigned short (number of vertices)
            //             + 3 x float (vertex coordinates) x (number of vertices)
            //             + sub chunks
            //-------------------------------------------
            case 0x4110:
                {
                uint16_t qty;
                read_or_fail(input_file, &qty, 1, sizeof(uint16_t));
                object->vertices.resize(qty);

                for (uint16_t i = 0; i < qty; i++) {
                    float f[3];
                    read_or_fail(input_file, f, 3, sizeof(float));
                    vec3& vertex = object->vertices[i].v;
                    vertex.x(f[0]);
                    vertex.y(f[1]);
                    vertex.z(f[2]);
                }
                }
                break;

            //--------------- TRI_FACEL1 ----------------
            // Description: Polygons (faces) list
            // Chunk ID: 4120 (hex)
            // Chunk Length: 1 x unsigned short (number of polygons)
            //             + 3 x unsigned short (polygon points) x (number of polygons)
            //             + sub chunks
            //-------------------------------------------
            case 0x4120:
                {
                uint16_t qty;
                read_or_fail(input_file, &qty, 1, sizeof(uint16_t));
                object->faces.resize(qty);
                for (uint16_t i = 0; i < qty; i++) {
                    uint16_t f[4];
                    read_or_fail(input_file, f, 4, sizeof(uint16_t));
                    uvec3& face = object->faces[i].v;
                    face.x(f[0]);
                    face.y(f[1]);
                    face.z(f[2]);
                }
                }
                break;

            //------------- TRI_MAPPINGCOORS ------------
            // Description: Vertices list
            // Chunk ID: 4140 (hex)
            // Chunk Length: 1 x unsigned short (number of mapping points)
            //             + 2 x float (mapping coordinates) x (number of mapping points)
            //             + sub chunks
            //-------------------------------------------
            case 0x4140:
                {
                uint16_t qty;
                read_or_fail(input_file, &qty, 1, sizeof(uint16_t));
                for (uint16_t i = 0; i < qty; i++) {
                    float f[2];
                    read_or_fail(input_file, f, 2, sizeof(float));
                    vec2& texcoord = object->vertices[i].t;
                    texcoord.x(f[0]);
                    texcoord.y(f[1]);
                }
                }
                gotTexcoords_ = true;
                break;

            //----------- Skip unknow chunks ------------
            //We need to skip all the chunks that currently we don't use
            //We use the chunk length information to set the file pointer
            //to the same level next chunk
            //-------------------------------------------
            default:
                input_file.seekg(chunk_length - 6, std::ios::cur);
        }
    }

    // Compute bounding box for perspective projection
    compute_bounding_box(*object);


    return true;
}


const unsigned int Model::Face::OBJ_FACE_V = 0x1;
const unsigned int Model::Face::OBJ_FACE_T = 0x2;
const unsigned int Model::Face::OBJ_FACE_N = 0x4;

/**
 * Parse 2-element vertex attribute from an OBJ file.
 *
 * @param source the source line to parse
 * @param v the vec2 to populate
 */
void
Model::obj_get_attrib(const string& source, vec2& v)
{
    // Our attribs are whitespace separated, so use a fuzzy split.
    vector<string> elements;
    Util::split(source, ' ', elements, Util::SplitModeFuzzy);

    // Find the first value...
    float x = Util::fromString<float>(elements[0]);
    // And the second value (there might be a third, but we don't care)...
    float y = Util::fromString<float>(elements[1]);
    v.x(x);
    v.y(y);
}

/**
 * Parse 3-element vertex attribute from an OBJ file.
 *
 * @param source the source line to parse
 * @param v the vec3 to populate
 */
void
Model::obj_get_attrib(const string& source, vec3& v)
{
    // Our attribs are whitespace separated, so use a fuzzy split.
    vector<string> elements;
    Util::split(source, ' ', elements, Util::SplitModeFuzzy);

    // Find the first value...
    float x = Util::fromString<float>(elements[0]);
    // Then the second value...
    float y = Util::fromString<float>(elements[1]);
    // And the third value (there might be a fourth, but we don't care)...
    float z = Util::fromString<float>(elements[2]);
    v.x(x);
    v.y(y);
    v.z(z);
}


void
Model::obj_face_get_index(const string& tuple, unsigned int& which,
    unsigned int& v, unsigned int& t, unsigned int& n)
{
    // We can use a normal split here as syntax requires no spaces around
    // the '/' delimiter for a face description.
    vector<string> elements;
    Util::split(tuple, '/', elements, Util::SplitModeNormal);

    if (elements.empty())
    {
        which = 0;
        return;
    }

    which = Face::OBJ_FACE_V;
    v = Util::fromString<unsigned int>(elements[0]);

    unsigned int num_elements = elements.size();

    if (num_elements > 1 && !elements[1].empty())
    {
        which |= Face::OBJ_FACE_T;
        t = Util::fromString<unsigned int>(elements[1]);
    }

    if (num_elements > 2 && !elements[2].empty())
    {
        which |= Face::OBJ_FACE_N;
        n = Util::fromString<unsigned int>(elements[2]);
    }

    return;
}

/**
 * Parse a face description from an OBJ file.
 * Faces always specify position, but optionally can also contain separate
 * indices for texcoords and normals.
 *
 * @param source the source line to parse
 * @param v the uvec3 to populate
 */
void
Model::obj_get_face(const string& source, Face& f)
{
    // Our indices are whitespace separated, so use a fuzzy split.
    vector<string> elements;
    Util::split(source, ' ', elements, Util::SplitModeFuzzy);

    // Find the first value...
    unsigned int which(0);
    unsigned int vx(0);
    unsigned int tx(0);
    unsigned int nx(0);
    obj_face_get_index(elements[0], which, vx, tx, nx);

    // Then the second value...
    unsigned int vy(0);
    unsigned int ty(0);
    unsigned int ny(0);
    obj_face_get_index(elements[1], which, vy, ty, ny);

    // And the third value (there might be a fourth, but we don't care)...
    unsigned int vz(0);
    unsigned int tz(0);
    unsigned int nz(0);
    obj_face_get_index(elements[2], which, vz, tz, nz);

    // OBJ models start absoluted indices at '1', so subtract to re-base to
    // '0'.  We do not handle relative indexing (negative indices).  
    f.which = which;
    f.v.x(vx - 1);
    f.v.y(vy - 1);
    f.v.z(vz - 1);
    if (which & Face::OBJ_FACE_T)
    {
        f.t.x(tx - 1);
        f.t.y(ty - 1);
        f.t.z(tz - 1);
    }
    if (which & Face::OBJ_FACE_N)
    {
        f.n.x(nx - 1);
        f.n.y(ny - 1);
        f.n.z(nz - 1);
    }
}


bool Model::load_mtl(const std::filesystem::path& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << filename << std::endl;
        return false;
    }

    std::string line;
    Material currentMaterial;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "newmtl") {
            if (!currentMaterial.name.empty()) {
                materials_[currentMaterial.name] = currentMaterial;
            }
            iss >> currentMaterial.name;
        } else if (key == "Ka") {
            iss >> currentMaterial.Ka[0] >> currentMaterial.Ka[1] >> currentMaterial.Ka[2];
        } else if (key == "Kd") {
            iss >> currentMaterial.Kd[0] >> currentMaterial.Kd[1] >> currentMaterial.Kd[2];
        } else if (key == "Ks") {
            iss >> currentMaterial.Ks[0] >> currentMaterial.Ks[1] >> currentMaterial.Ks[2];
        } else if (key == "Ns") {
            iss >> currentMaterial.Ns;
        } else if (key == "d") {
            iss >> currentMaterial.d;
        }
    }

    if (!currentMaterial.name.empty()) {
        materials_[currentMaterial.name] = currentMaterial;
    }

    return true;
}

const Material* Model::getMaterialByName(const std::string& name) const {
    auto it = materials_.find(name);
    if (it != materials_.end()) {
        return &(it->second);
    }
    return nullptr;
}


/**
 * Load a model from an OBJ file.
 *
 * @param filename the name of the file
 *
 * @return whether loading succeeded
 */
bool Model::load_obj(const std::filesystem::path &filename) {
    const std::unique_ptr<std::istream> input_file_ptr(Util::get_resource(filename));
    std::istream& inputFile(*input_file_ptr);
    if (!inputFile) {
        return false;
    }

    // Parse MTL file
    //std::string mtlFilename = filename.replace_extension(".mtl").string();
    std::filesystem::path mutableFilename = filename; // Copy filename to a non-const variable
    std::string mtlFilename = mutableFilename.replace_extension(".mtl").string();


    load_mtl(mtlFilename);

    vector<string> sourceVec;
    string curLine;
    while (getline(inputFile, curLine)) {
        sourceVec.push_back(curLine);
    }

    objects_.clear();  // Clear existing objects before loading

    static const string group_definition("g");
    static const string vertex_definition("v");
    static const string normal_definition("vn");
    static const string texcoord_definition("vt");
    static const string face_definition("f");

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texcoords;

    Object *currentObject = nullptr;

    for (const string& curSrc : sourceVec) {
        string::size_type startPos(0);
        string::size_type spacePos = curSrc.find(" ", startPos);
        string::size_type num_chars(string::npos);
        string definition;
        if (spacePos != string::npos) {
            string::size_type defPos = curSrc.find_first_not_of(' ', spacePos);
            definition = string(curSrc, defPos);
            num_chars = spacePos - startPos;
        }
        string definitionType(curSrc, startPos, num_chars);

        if (definitionType == group_definition) {
            // Start a new object/group
            if (currentObject != nullptr && !currentObject->faces.empty()) {
                objects_.push_back(*currentObject);
            }
            currentObject = new Object(definition);  // Start a new object for the group
        } else if (definitionType == vertex_definition) {
            vec3 p;
            obj_get_attrib(definition, p);
            positions.push_back(p);
        } else if (definitionType == normal_definition) {
            vec3 n;
            obj_get_attrib(definition, n);
            normals.push_back(n);
        } else if (definitionType == texcoord_definition) {
            vec2 t;
            obj_get_attrib(definition, t);
            texcoords.push_back(t);
        } else if (definitionType == face_definition && currentObject != nullptr) {
            Face f;
            obj_get_face(definition, f);
            currentObject->faces.push_back(f);
        }
    }

    // Add the last object found (if any)
    if (currentObject != nullptr && !currentObject->faces.empty()) {
        objects_.push_back(*currentObject);
    }

    if (!texcoords.empty()) {
        gotTexcoords_ = true;
    }
    if (!normals.empty()) {
        gotNormals_ = true;
    }

    // Assign vertices to objects
    for (Object& object : objects_) {
        unsigned int numVertices = positions.size();
        object.vertices.resize(numVertices);
        for (unsigned int i = 0; i < numVertices; i++) {
            Vertex& curVertex = object.vertices[i];
            curVertex.v = positions[i];
            if (gotTexcoords_) {
                curVertex.t = texcoords[i];
            }
            if (gotNormals_) {
                curVertex.n = normals[i];
            }
        }
        // Compute bounding box for perspective projection
        compute_bounding_box(object);
    }

    return true;
}

namespace ModelPrivate
{
ModelMap modelMap;
}

/**
 * Locate all available models.
 *
 * This method scans the built-in data paths and build a database of usable
 * models available to scenes.  Map is available on a read-only basis to scenes
 * that might find it useful for listing models, etc.
 *
 * @return a map containing information about the located models
 */
const ModelMap&
Model::find_models()
{
    if (!ModelPrivate::modelMap.empty())
    {
        return ModelPrivate::modelMap;
    }

    // Set the directory where models are located
    string dataDir = "./models"; // Use the current directory

    // realpath() requires a buffer of at least PATH_MAX bytes; with
    // _FORTIFY_SOURCE enabled, passing a smaller buffer aborts the program
    // ("buffer overflow detected") via __realpath_chk.
    char fullPath[PATH_MAX];
    if (realpath(dataDir.c_str(), fullPath) != NULL) {
        printf("Scanning directory: %s\n", fullPath);
    } else {
        perror("realpath() error");
        return ModelPrivate::modelMap;
    }

    // List all files in the directory
    vector<std::filesystem::path> pathVec;
    Util::list_files(dataDir, pathVec);

    // Explicitly check for horse.3ds
    std::filesystem::path horsePath = std::filesystem::absolute(dataDir + "/horse.obj");
    printf("Checking for horse.3ds at path: %s\n", horsePath.c_str());
    if (std::filesystem::exists(horsePath))
    {
        printf("Adding horse.3ds to model map\n");
        std::unique_ptr<ModelDescriptor> desc(new ModelDescriptor("audi", MODEL_3DS, horsePath));
        ModelPrivate::modelMap.insert(std::make_pair("audi", std::move(desc)));
    }
    else
    {
        printf("horse.3ds not found at path: %s\n", horsePath.c_str());
    }

    // Automatically add other models found in the directory
    for (const auto &curPath : pathVec)
    {
        auto ext = curPath.extension();
        auto name = curPath.stem().string();

        printf("Found file: %s\n", curPath.c_str());

        ModelFormat format(MODEL_INVALID);
        if (ext == ".3ds")
            format = MODEL_3DS;
        else if (ext == ".obj")
            format = MODEL_OBJ;

        if (format != MODEL_INVALID) {
            printf("Adding model to map: %s (format: %d)\n", name.c_str(), format);
            std::unique_ptr<ModelDescriptor> desc(new ModelDescriptor(name, format, curPath));
            ModelPrivate::modelMap.insert(std::make_pair(name, std::move(desc)));
        }
    }

    return ModelPrivate::modelMap;
}

/**
 * Load a model by name.
 *
 * You must initialize the available model collection using
 * Model::find_models() before using this method.
 *
 * @param modelName the model name
 *
 * @return whether the operation succeeded
 */
bool Model::load(const string& modelName)
{
    bool retVal(false);
    ModelMap::const_iterator modelIt = ModelPrivate::modelMap.find(modelName);
    if (modelIt == ModelPrivate::modelMap.end())
    {
        printf("Model not found in the model map: %s\n", modelName.c_str());
        return retVal;
    }

    ModelDescriptor* desc = modelIt->second.get();
    printf("Loading model from path: %s\n", desc->pathname().c_str()); // Add this line

    switch (desc->format())
    {
        case MODEL_INVALID:
            printf("Invalid model format for model: %s\n", modelName.c_str());
            break;
        case MODEL_3DS:
            retVal = load_3ds(desc->pathname());
            break;
        case MODEL_OBJ:
            retVal = load_obj(desc->pathname());
            break;
    }

    if (retVal) {
        printf("Model loaded successfully: %s\n", modelName.c_str());
    } else {
        printf("Failed to load model: %s\n", modelName.c_str());
    }

    return retVal;
}
