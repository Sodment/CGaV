#include "particle_system.h"

ParticleGenerator::ParticleGenerator(GLuint texture, glm::vec3 pos, unsigned int amount)
{
	this->position = pos;
	this->texture = texture;
	this->amount = amount;
	particles.resize(amount);
	positions.resize(4 * amount);
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		// give every particle a random position
		particles[i] = RespawnParticle();
	}

	glGenBuffers(1, &VAO);
	glGenBuffers(1, &positionsBuffer);

	float verticies[] =
	{
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 0.0f  
	};

	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), &verticies, GL_STATIC_DRAW);

	// fill the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * 5 * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
}

void ParticleGenerator::Update(float dt)
{
	for (size_t i = 0; i < particles.size(); ++i)
	{
		// subtract from the particles lifetime
		this->particles[i].Life -= dt;

		// if the lifetime is below 0 respawn the particle
		if (this->particles[i].Life <= 0.0f)
		{
			particles[i] = RespawnParticle();
		}

		// move the particle down depending on the delta time
		this->particles[i].Position += glm::vec3(0.0f, dt * 2.0f, 0.0f);

		// update the position buffer
		this->positions[i * 4 + 0] = this->particles[i].Position[0];
		this->positions[i * 4 + 1] = this->particles[i].Position[1];
		this->positions[i * 4 + 2] = this->particles[i].Position[2];
		this->positions[i * 4 + 3] = this->particles[i].Life;
	}
}

void ParticleGenerator::Draw(ShaderProgram& shader)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "sprite"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE0);

	// update the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->particles.size() * 5 * sizeof(float), positions.data());

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(4, 1);

	// draw triangles
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(4);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printf("IM HERE\n");
}

Particle ParticleGenerator::RespawnParticle()
{
	std::random_device rd;

	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0.5, 2);
	Particle part;
	float r = dist(e2);
	float r2 = dist(e2);
	part.Position = position + glm::vec3(r, 0.0, 0.1*r);
	part.Life = r2;
	return part;
}


