#version 330 core

uniform uvec4 uuid;
out uvec4 outUuid;

void main()
{
    outUuid = uuid;
}