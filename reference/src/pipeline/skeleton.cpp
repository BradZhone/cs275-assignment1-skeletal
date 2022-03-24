#include "pipeline/skeleton.hpp"

WireframeSkeletonPipeline::
WireframeSkeletonPipeline(
    Shader* _shader, 
    FirstPersonCamera* _camera,
    Skeleton* _skel
) {
    glGenVertexArrays(1, &this->glo.VAO); // Allocate a Vertex Array Object to manage data
    glGenBuffers(1, &this->glo.VBO); // Allocate a Vertex Buffer Object to save vertex data
    glGenBuffers(1, &this->glo.EBO); // Allocate an Element Buffer Object to save indices data

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glo.EBO); // Bound to VAO
    
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO); // WARN: A global binding instead of binding to VAO

    // Define how to send data into layout 0
    // Check "res/shader/skeleton.vs"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    
    glBindVertexArray(0); // unbind VAO

    this->shader = _shader;
    this->camera = _camera;
    this->skeleton = _skel;

    this->setupIndices();
}

void WireframeSkeletonPipeline::
setupIndices() {
    // Load indices to render a skeleton
    const auto& joints = this->skeleton->getJoints();

    this->indices.clear();
    this->vertices.resize(joints.size());

    // GL_LINES mode, two indice as a line
    for (int i = 0; i < joints.size(); ++i) {
        /****************************************My Code***************************************************/
        if(joints[i].Parent!=-1)
        {
            this->indices.push_back(i);
            // this->indices.push_back(`the_parent_of_i`);
            this->indices.push_back(joints[i].Parent);
        }
        /****************************************My Code end***************************************************/
    }

    // Copy the indice to EBO since they are fixed
    glBindVertexArray(this->glo.VAO); // EBO is bound to VAO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0); // unbind VAO
}

void WireframeSkeletonPipeline::
updateVertices() {
    // get position of each joint from skeletal animator at current frame
     /****************************************My Code***************************************************/
    const auto& joints = this->skeleton->getJoints();
     /****************************************My Code end***************************************************/

    for (size_t i = 0; i < this->vertices.size(); ++i) {
        /*
            this->vertices[i].position.x = `new_x`;
            this->vertices[i].position.y = `new_y`;
            this->vertices[i].position.z = `new_z`;
        */
       /****************************************My Code***************************************************/
        this->vertices[i].position.x = joints[i].position.x;
        this->vertices[i].position.y = joints[i].position.y;
        this->vertices[i].position.z = joints[i].position.z;
        /****************************************My Code end***************************************************/
    }
}

void WireframeSkeletonPipeline::
draw() {

    GLint previous;
    // glGetIntegerv(GL_POLYGON_MODE, &previous); // save previous drawing mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // we want wire-frame mode

    glBindVertexArray(this->glo.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->glo.VBO);

    // update and copy the new vertex data into VBO
    updateVertices();
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexData), this->vertices.data(), GL_STREAM_DRAW);

    this->shader->use();
    this->camera->applyToShader(this->shader);

    // Draw using indices 
    glDrawElements(GL_LINES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Hint: when drawing mesh, you might want to use a draw mode of GL_TRIANGLES
    // For other drawing modes, check
    // https://www.evl.uic.edu/julian/cs488/2005-09-01/fig2-6.gif

    // glPolygonMode(GL_FRONT_AND_BACK, previous); // restore previous drawing mode
}