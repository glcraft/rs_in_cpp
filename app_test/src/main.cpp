#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <rust/Result.h>
#include <rust/Iterators.h>
#include <rust/Macros.h>
int main()
{
    using namespace rust;
    {
        auto vec = std::vector{0,1,2,3,4,5,6};
        auto it = iter(vec);
        auto it_map = it
            .filter([](int val) -> bool { return val%2==0; })
            .map([](int val) -> double { return static_cast<double>(val)*5.5; })
            .enumerate();
        auto value = *it_map;
        while (value.has_value())
        {
            auto [i, v] = value.value();
            std::cout << i << ", " << v << '\n';
            ++it_map;
            value = *it_map;
        }
    }
    exit(0);
    // Ok assertion
    {
        auto r = rust::Result<int, int>::Ok(1);
        assert(r.is_ok());
    }
    //Err assertion
    {
        auto r = rust::Result<int, int>::Err(1);
        assert(r.is_err());
    }
    // test as_ref
    {
        auto r = rust::Result<std::string, int>::Ok("hello");
        auto r_ok = r.ok();
        assert(r_ok.has_value() && r_ok.value() == "hello");
    }
    //test is_ok
    {
        Result<int32_t, std::string_view> x = Result<int32_t, std::string_view>::Ok(-3);
        assert_eq(x.is_ok(), true);

        Result<int32_t, std::string_view> y = Result<int32_t, std::string_view>::Err("Some error message");
        assert_eq(y.is_ok(), false);
    }
    //test is_err
    {
        Result<int32_t, std::string_view> x = Result<int32_t, std::string_view>::Ok(-3);
        assert_eq(x.is_err(), false);

        Result<int32_t, std::string_view> y = Result<int32_t, std::string_view>::Err("Some error message");
        assert_eq(y.is_err(), true);
    }
    //test ok
    {
        Result<uint32_t, std::string_view> x = Result<uint32_t, std::string_view>::Ok(2);
        assert_eq(x.ok().value_or(0), 2);
        Result<uint32_t, std::string_view> y = Result<uint32_t, std::string_view>::Err("Nothing here");
        assert_eq(y.ok().has_value(), false);
    }
    //test err
    {
        using r_t = Result<int32_t, std::string_view>;
        r_t x = r_t::Ok(2);
        assert_eq(x.err().has_value(), false);
        r_t y = r_t::Err("Nothing here");
        assert_eq(y.err().value_or(""), "Nothing here");
    }
    //test map
    {
        using r_t = Result<int32_t, std::string_view>;
        r_t x = r_t::Ok(2);
        assert_eq(x.map<int>([](auto v){ return v*2; }).ok().value(), 4);
        r_t y = r_t::Err("Nothing here");
        assert_eq(y.map<int>([](auto v){ return v*2; }).err().value(), "Nothing here");
    }
    //test map_or    
    {
        auto x = Result<std::string_view, std::string_view>::Ok("foo");
        assert_eq(x.map_or<size_t>(42, [](auto v){ return v.length(); }), 3);
        auto y = Result<std::string_view, std::string_view>::Err("foo");
        assert_eq(y.map_or<size_t>(42, [](auto v){ return v.length(); }), 42);
    }
    //test map_or_else
    {
        
        size_t k = 21;
        auto x = Result<std::string_view, std::string_view>::Ok("foo");
        assert_eq(x.map_or_else<size_t>([k](auto e){ return k*2; }, [](auto v){ return v.length(); }), 3);
        auto y = Result<std::string_view, std::string_view>::Err("foo");
        assert_eq(y.map_or_else<size_t>([k](auto e){ return k*2; }, [](auto v){ return v.length(); }), 42);
    }
    //test map_err
    {
        auto stringify = [](int i) -> std::string {
            return "error code: "+std::to_string(i);
        };
        auto x = Result<int, int>::Ok(2);
        assert_eq(x.map_err<std::string>(stringify).ok().value(), 2);
        auto y = Result<int, int>::Err(13);
        assert_eq(y.map_err<std::string>(stringify).err().value(), "error code: 13");
    }
    //test and
    {
        auto x1 = Result<int, std::string_view>::Ok(2);
        auto y1 = Result<std::string_view, std::string_view>::Err("late error");
        assert_eq(x1.and_r(y1).err().value(), "late error");

        auto x2 = Result<int, std::string_view>::Err("early error");
        auto y2 = Result<std::string_view, std::string_view>::Ok("foo");
        assert_eq(x2.and_r(y2).err().value(), "early error");

        auto x3 = Result<int, std::string_view>::Err("not a 2");
        auto y3 = Result<std::string_view, std::string_view>::Ok("late error");
        assert_eq(x3.and_r(y3).err().value(), "not a 2");

        auto x4 = Result<int, std::string_view>::Ok(2);
        auto y4 = Result<std::string_view, std::string_view>::Ok("different result type");
        assert_eq(x4.and_r(y4).ok().value(), "different result type");
    }
    //test and_then
    {
        using r_t = Result<int, int>;
        auto sq = [](int i) -> r_t{ return r_t::Ok(i*i); };
        auto err = [](int i) -> r_t{ return r_t::Err(i); };
        assert_eq(r_t::Ok(2).and_then<int>(sq).and_then<int>(sq).ok().value(), 16);
        assert_eq(r_t::Ok(2).and_then<int>(sq).and_then<int>(err).err().value(), 4);
        assert_eq(r_t::Ok(2).and_then<int>(err).and_then<int>(sq).err().value(), 2);
        assert_eq(r_t::Ok(3).and_then<int>(err).and_then<int>(err).err().value(), 3);
    }
    //test or
    {
        using r_t = Result<int, std::string_view>;
        auto x1 = r_t::Ok(2);
        auto y1 = r_t::Err("late error");
        assert_eq(x1.or_r<std::string_view>(y1).ok().value(), 2);

        auto x2 = r_t::Err("early error");
        auto y2 = r_t::Ok(2);
        assert_eq(x2.or_r<std::string_view>(y2).ok().value(), 2);

        auto x3 = r_t::Err("not a 2");
        auto y3 = r_t::Err("late error");
        assert_eq(x3.or_r<std::string_view>(y3).err().value(), "late error");
        
        auto x4 = r_t::Ok(2);
        auto y4 = r_t::Ok(100);
        assert_eq(x4.or_r<std::string_view>(y4).ok().value(), 2);
    }
    //test or_else
    {
        using r_t = Result<int, int>;
        auto sq = [](int i) -> r_t{ return r_t::Ok(i*i); };
        auto err = [](int i) -> r_t{ return r_t::Err(i); };
        assert_eq(r_t::Ok(2).or_else<int>(sq).or_else<int>(sq).ok().value(), 2);
        assert_eq(r_t::Ok(2).or_else<int>(err).or_else<int>(sq).ok().value(), 2);
        assert_eq(r_t::Err(3).or_else<int>(sq).or_else<int>(err).ok().value(), 9);
        assert_eq(r_t::Err(3).or_else<int>(err).or_else<int>(err).err().value(), 3);
    }
    //test unwrap_or
    {
        int def = 2;
        auto x = Result<int, std::string_view>::Ok(9);
        assert_eq(x.unwrap_or(def), 9);

        auto y = Result<int, std::string_view>::Err("error");
        assert_eq(y.unwrap_or(def), def);
    }
    //test unwrap or else
    {
        using r_t = Result<size_t, std::string_view>;
        auto count = [](std::string_view err) -> size_t { return err.length(); };
        assert_eq(r_t::Ok(2).unwrap_or_else(count), 2);
        assert_eq(r_t::Err("foo").unwrap_or_else(count), 3);
    }
    return EXIT_SUCCESS;
}