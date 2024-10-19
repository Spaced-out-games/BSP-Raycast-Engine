#pragma once

// Just a basic entity
struct ent
{
	ent() = default;
	~ent() = default;
	// Not implemented; inherit from `ent`
	virtual void draw() {};

	// Not implemented; inherit from `ent`
	virtual void init() {};

	// Not implemented; inherit from `ent`
	virtual void tick() {};
	
};

