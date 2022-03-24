/****************************************My Code***************************************************/
#include "pipeline/mesh.hpp"
#include <string>

WireframeMeshPipeline::
WireframeMeshPipeline(
    Shader* _shader, 
    FirstPersonCamera* _camera,
    BoneWeightedMesh* _mesh
) {
    this->indices = _mesh->indices;
    this->vertices.resize(_mesh->positions.size());
    // Read positions, normals, uvs, influences, weights from _mesh
    for (int i = 0; i<_mesh->positions.size(); i++)
    {
        // VertexData temp;
        this->vertices[i].positions = _mesh->positions[i];
        // this->vertices[i].normals = _mesh->normals[i];
        // this->vertices[i].influences = _mesh->influences[i];
        // this->vertices[i].weights = _mesh->weights[i];
    }
    glGenVertexArrays(1, &this->glo.VAO); // Allocate a Vertex Array Object to manage data
    glGenBuffers(1, &this->glo.VBO); // Allocate a Vertex Buffer Object to save vertex data
    glGenBuffers(1, &this->glo.EBO); // Allocate an Element Buffer Object to save indices data

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO); // WARN: A global binding instead of binding to VAO
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexData), &this->vertices[0], GL_STREAM_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glo.EBO); // Bound to EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STREAM_DRAW);
    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    // // vertex normals
    // glEnableVertexAttribArray(1);	
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normals));
    // // vertex texture coords
    // glEnableVertexAttribArray(2);	
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uvs));
    // // ids
    // glEnableVertexAttribArray(3);
    // glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexData), (void*)offsetof(VertexData, influences));
    // // weights
    // glEnableVertexAttribArray(4);
    // glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, weights));
    
    glBindVertexArray(0);

    this->shader = _shader;
    this->camera = _camera;

}

void WireframeMeshPipeline::
draw() {
    // GLint previous;
    // glGetIntegerv(GL_POLYGON_MODE, &previous); // save previous drawing mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // we want wire-frame mode

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO);

    this->shader->use();
    this->camera->applyToShader(this->shader);

    // Draw using indices 
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(this->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, previous); // restore previous drawing mode
}

/****************************************My Code end***************************************************/