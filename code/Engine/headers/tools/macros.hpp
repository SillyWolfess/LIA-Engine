#ifndef LIA_MACROSS_HPP
#define LIA_MACROSS_HPP

#include "./../logs.hpp"

#define LIA_TRY try {
//#define LIA_CATCH_RETURN_FALSE } catch (std::exception const& ex) { LIA_fatal(std::vformat("{}", std::make_format_args(ex.what()))); return false;} catch (...) { LIA_fatal("EXCEPTION CAUGHT"); return false; }
//#define LIA_CATCH(errorCode) } catch (std::exception const& ex) { LIA_fatal(std::vformat("{}", std::make_format_args(ex.what()))); errorCode} catch (...) { LIA_fatal("EXCEPTION CAUGHT"); errorCode }
//#define LIA_CATCH_EMPTY } catch (std::exception const& ex) { LIA_fatal(std::vformat("{}", std::make_format_args(ex.what())));} catch (...) { LIA_fatal("EXCEPTION CAUGHT");}

#define LIA_CATCH_RETURN_FALSE } catch (std::exception const& ex) { LIA_fatal_f("{}", ex.what()); return false;} catch (...) { LIA_fatal("EXCEPTION CAUGHT"); return false; }
#define LIA_CATCH(errorCode) } catch (std::exception const& ex) { LIA_fatal_f("{}", ex.what()); errorCode} catch (...) { LIA_fatal("EXCEPTION CAUGHT"); errorCode }
#define LIA_CATCH_EMPTY } catch (std::exception const& ex) { LIA_fatal_f("{}", ex.what());} catch (...) { LIA_fatal("EXCEPTION CAUGHT");}
#define LIA_ASSERT(condition, msg) if (!(condition)) { LIA_fatal(msg); std::abort();}
#endif