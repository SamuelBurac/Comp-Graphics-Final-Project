 float segment_length = 0.1f;
        float segment_width = 0.01f;
        float angle = 25.0f * M_PI / 180.0f;

        std::vector<Vector3> vertices;
        std::vector<Vector3i> elements;
        std::stack<std::tuple<Vector3, Vector3>> state_stack;

        Vector3 pos(p.x, p.y , p.z);
        Vector3 dir(0, 1, 0);

        std::string current = "V";
        for (int i = 0; i < iterations; i++)
        {
            std::string next;
            for (char c : current)
            {
                switch (c)
                {
                case 'V':
                    next += "[+++W][---W]F[++W][--W]F[+W][-W]YV";
                    break;
                case 'W':
                    next += "+X[-W]Z";
                    break;
                case 'X':
                    next += "-W[+X]Z";
                    break;
                case 'Y':
                    next += "Y[++W][--W]Z";
                    break;
                case 'Z':
                    next += "[-FFF][+FFF]F";
                    break;
                case 'F':
                    next += "F";
                    break;
                default:
                    next += c;
                }
            }
            current = next;
        }

        int vertex_count = 0;
        for (char c : current)
        {
            Vector3 next_pos;
            switch (c)
            {
            case '+':
                dir = Vector3(dir.x() * cos(angle) - dir.y() * sin(angle),
                              dir.x() * sin(angle) + dir.y() * cos(angle),
                              dir.z());
                break;
            case '-':
                dir = Vector3(dir.x() * cos(-angle) - dir.y() * sin(-angle),
                              dir.x() * sin(-angle) + dir.y() * cos(-angle),
                              dir.z());
                break;
            case '[':
                state_stack.push({pos, dir});
                break;
            case ']':
                std::tie(pos, dir) = state_stack.top();
                state_stack.pop();
                break;

            default:
                if (c == 'F' ||c == 'X' || c == 'W' || c == 'Y'|| c == 'V' || c == 'Z')
                {
                    next_pos = pos + dir * segment_length;
                    Vector3 perp(-dir.y(), dir.x(), 0); // Perpendicular to direction

                    // Create rectangle vertices
                    Vector3 v0 = pos + perp * segment_width;
                    Vector3 v1 = pos - perp * segment_width;
                    Vector3 v2 = next_pos + perp * segment_width;
                    Vector3 v3 = next_pos - perp * segment_width;



                    vertices.push_back(v0);
                    vertices.push_back(v1);
                    vertices.push_back(v2);
                    vertices.push_back(v3);


                    // Two triangles for rectangle
                    elements.push_back(Vector3i(vertex_count, vertex_count + 1, vertex_count + 2));
                    elements.push_back(Vector3i(vertex_count + 1, vertex_count + 3, vertex_count + 2));

                    vertex_count = vertex_count + 4;
                    pos = next_pos;
                    break;
                }
            }
        }

        return {vertices, elements};