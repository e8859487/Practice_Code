using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication4_LINQ
{
    class Linq_Basic_Concept
    {
        
    
    }
    class Linq_A
    {
        public void display()
        {
            //List 物件實做了 IEnumerable<T> 介面  , 所以它能夠引用 where 方法
            //但是實際上實做的還是 Enumerable 類別去完成where的方法的
            List<string> weekList = new List<string> { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
            foreach (string str in weekList)
            {
                Console.WriteLine(str);
            }

            Console.WriteLine("===================================");


            IEnumerable<string> eWeekList = weekList.Where(weekday => weekday.StartsWith("T"));
            foreach (string str in eWeekList)
            {
                Console.WriteLine(str);
            }
        }
    }
}
