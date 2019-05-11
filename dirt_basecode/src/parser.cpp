/*
    This file is part of Dirt, the Dartmouth introductory ray tracer, used in
    Dartmouth's COSC 77/177 Computer Graphics course.

    Copyright (c) 2018 by Wojciech Jarosz

    Dirt is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Dirt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser.h"
#include <fstream>
#include <iomanip>

// local functions
namespace
{

// parse value
json cmdlineParseValue(const string & str, const CommandLine::Argument & opt, const CommandLine & cmd)
{
    try
    {
        if (typeid(int) == opt.type)
        {
            return json(std::stoi(str));
        }
        else if (typeid(float) == opt.type)
        {
            return json(std::stod(str));
        }
        else if (typeid(double) == opt.type)
        {
            return json(std::stod(str));
        }
        else if (typeid(string) == opt.type)
        {
            return json(str);
        }
        else
            error("unknown type");
    }
    catch (const std::exception &)
    {
        cmd.printUsageAndQuit(tfm::format("invalid value for %s", opt.name.c_str()));
    }
    return json();
}

string typeString(const CommandLine::Argument & opt)
{
	if (typeid(int) == opt.type)
		return "int";
	else if (typeid(float) == opt.type ||
			 typeid(double) == opt.type)
		return "real";
	else if (typeid(string) == opt.type)
		return "string";
	else if (typeid(bool) == opt.type)
		return "";

	error("unknown type");
	return "unknown";
}

Eigen::Matrix4f parseSingleTransform(const json & j)
{
    Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
    if (j.count("from") || j.count("at") || j.count("to") || j.count("up"))
    {
        Vector3f from = Vector3f::UnitZ(), at = Vector3f::Zero(), up = Vector3f::UnitY();
        from = j.value("from", from);
	    at = j.value("at", at) + j.value("to", at);
	    up = j.value("up", up);

        Vector3f dir = (from - at).normalized();
        Vector3f left = up.normalized().cross(dir).normalized();
        Vector3f newUp = dir.cross(left).normalized();

        m.topLeftCorner<3,4>() << left, newUp, dir, from;
        return m;
    }
    else if (j.count("o") || j.count("x") || j.count("y") || j.count("z"))
    {
        Vector3f o(0,0,0), x(1, 0, 0), y(0, 1, 0), z(0, 0, 1);
        o = j.value("o", o);
        x = j.value("x", x);
        y = j.value("y", y);
        z = j.value("z", z);
        m.topLeftCorner<3,4>() << x, y, z, o;
        return m;
    }
    else if (j.count("translate"))
    {
        Eigen::Vector3f v = j["translate"];
        m.topRightCorner<3,1>() << v;
        return m;
    }
    else if (j.count("scale"))
    {
        Vector3f v(1,1,1);
        if (j.size() == 1)
        {
            float s = j["scale"];
            v = Vector3f(s,s,s);
        }
        else if (j.size() == 3)
            v = j["scale"];

        m.diagonal().head<3>() << v;
        return m;
    }
    else if (j.count("axis") || j.count("angle"))
    {
        float angle = 0.0;
        Vector3f axis(1,0,0);
        axis = j.value("axis", axis);
        angle = j.value("angle", angle);
        m.topLeftCorner<3,3>() << Eigen::AngleAxis<float>(deg2rad(angle), axis).matrix();
        return m;
    }
    else if (j.count("matrix"))
    {
	    m = j["matrix"];
        return m;
    }
    else
        error("unrecognized \"transform\" command: %s.\n", j.dump());

    return m;
}

} // namespace


void from_json(const json & j, Transform & v)
{
    Eigen::Matrix4f m = v.getMatrix();
    if (j.is_array())
    {
        // multiple transformation commands listed in order
        for (auto & element : j)
            m = parseSingleTransform(element) * m;
    }
    else if (j.is_object())
    {
        // a single transformation
        m = parseSingleTransform(j);
    }
    else
        error("\"transform\" command must be either an array or an object.");

    v = Transform(m);
}


// json handling
json loadJSON(const string & filename)
{
    // open file
    std::ifstream stream(filename.c_str(), std::ifstream::in);
    if (!stream.good())
    	error("cannot open file: %s\n", filename.c_str());
    json j;
    stream >> j;
    stream.close();
    return j;
}


// print usage information
void CommandLine::printUsageAndQuit(const string & msg) const
{
    if (!msg.empty())
        error("%s\n\n", msg.c_str());

    int maxWidth = 25;

    cerr << rang::style::bold << "usage: " << rang::style::reset << progname;
    cerr << rang::fgB::cyan;
    for (auto opt : options)
    {
        cerr << " ";
        auto optname = (opt.flag == "") ? "--"+opt.name : "-"+opt.flag;
        auto optval = (opt.type != typeid(bool)) ? " <"+opt.name+">" : "";
        if (opt.opt)
            cerr << "[" << optname << optval << "]";
        else
        	cerr << optname << optval;
    }
    cerr << rang::fgB::green;
    for (auto arg : arguments)
    {
        cerr << " ";
        if (arg.opt)
            cerr << "[" << arg.name << "]";
        else
            cerr << "<" << arg.name << ">";
    }
    cerr << rang::fg::reset << endl << endl;

    if (!options.empty() || !arguments.empty())
    {
        cerr << rang::style::bold << "options:\n" << rang::style::reset;
        for (auto opt : options)
        {
            string optline = string("  ")
                           + ((opt.flag == "") ? "" : "-" + opt.flag + "/")
                           + "--" + opt.name
                           + ((opt.type != typeid(bool))
                                ? " <" + typeString(opt) + ">:"
                                : string(":"));
            cerr << rang::fgB::cyan
                 << std::left << std::setw(maxWidth) << optline
                 << rang::fg::reset;
            cerr << rang::style::italic
                 << opt.desc << "\n"
                 << rang::style::reset;
        }

	    cerr << rang::style::bold << "\narguments:\n" << rang::style::reset;
        for (auto arg : arguments)
        {
            string argline = string("  <") + arg.name + ">:";
            cerr << rang::fgB::green
                 << std::left << std::setw(maxWidth) << argline
                 << rang::fg::reset;
            cerr << rang::style::italic
                 << arg.desc << "\n"
                 << rang::style::reset;
        }
        cerr << "\n\n";
    }

	cerr << flush;

    exit(msg.empty() ? EXIT_SUCCESS : EXIT_FAILURE);
}


// parsing values
json CommandLine::parse(const vector<string> & args) const
{
    auto parsed = json::object_t();
    auto largs = args; // make a copy to change
    set<string> visited;
    for (auto opt : options)
    {
        auto pos = -1;
        for (auto i : range(int(largs.size())))
        {
            if (largs[i] == "--"+opt.name || largs[i] == "-"+opt.flag)
            {
                pos = i; break;
            }
        }
        if (pos < 0)
        {
            if (opt.opt)
                parsed[opt.name] = opt.def;
            else
                printUsageAndQuit(tfm::format("required option -%s", opt.flag.c_str()));
        }
        else
        {
            if (opt.type != typeid(bool))
            {
                if (pos == int(largs.size())-1)
                    printUsageAndQuit(tfm::format("no value for argument %s", opt.name.c_str()));
                auto sval = largs[pos+1];
                largs.erase(largs.begin()+pos,largs.begin()+pos+2);
                parsed[opt.name] = cmdlineParseValue(sval, opt, *this);
            }
            else
            {
                parsed[opt.name] = true;
                largs.erase(largs.begin()+pos);
            }
        }
    }
    for (auto arg : largs)
    {
        if (arg[0] == '-')
            printUsageAndQuit(tfm::format("unknown option %s", arg.c_str()));
    }
    for (auto arg : arguments)
    {
        if (largs.empty())
        {
            if (arg.opt)
                parsed[arg.name] = arg.def;
            else
                printUsageAndQuit(tfm::format("missing required argument %s", arg.name.c_str()));
        }
        else
        {
            parsed[arg.name] = cmdlineParseValue(largs[0], arg, *this);
            largs.erase(largs.begin());
        }
    }
    if (not largs.empty())
        printUsageAndQuit("too many arguments");
    return json(parsed);
}

// parsing values
json CommandLine::parse(int argc, char** argv) const
{
    auto args = vector<string>();
    for (auto i : range(1, argc))
        args.push_back(argv[i]);
    return parse(args);
}
